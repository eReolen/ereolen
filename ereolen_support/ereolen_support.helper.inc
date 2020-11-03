<?php

/**
 * @file
 * Contains helper functions for the support form.
 */

/**
 * Search for a drupal user email in Jira. We must make sure the email address
 * is not in use before attempting to create a user from it.
 *
 * @param \stdClass $user
 *   A drupal user.
 *
 * @return bool|mixed
 *   A jira user or FALSE if no user was found.
 *
 * @throws \Exception
 */
function _ereolen_support_search_jira_user($user, $form_state) {
  $form_email = $form_state['values']['ereolen_support_email'];
  $requestParts = _ereolen_support_get_request_parts('GET');
  $result = drupal_http_request($requestParts['jira_path'] . '/user/search?includeInactive=true&username=' . $form_email, $requestParts['options']);

  // Inspect result.
  if (!_ereolen_support_result_confirmed($result)) {
    return FALSE;
  };

  $resultArray = json_decode($result->data);

  // Searches for matches in full name, username and email, so we may have
  // multiple results.
  if (!empty($resultArray)) {
    foreach ($resultArray as $jiraUser) {
      if (is_object($jiraUser) && $jiraUser->name == $form_email) {
        // If user is deactivated in Jira we need to activate it.

        if ($jiraUser->active == false) {
          $jiraUser = _ereolen_support_change_jira_user_status($jiraUser, $form_state);
        }

        return $jiraUser;
      }
    }
  }

  return FALSE;
}

/**
 * Create a new Jira user.
 *
 * @param \stdClass $user
 *   A drupal user.
 * @param array $form_state
 *   The state of the form.
 *
 * @return mixed|null
 *   A newly created Jira user or Null if no user was created.
 *
 * @throws \Exception
 */
function _ereolen_support_create_jira_user($user, $form_state) {
  $form_email = $form_state['values']['ereolen_support_email'];
  $data = [
    'name' => $form_email,
    'emailAddress' => $form_email,
    'displayName' => $form_email,
  ];

  $requestParts = _ereolen_support_get_request_parts('POST', $data);

  $result = drupal_http_request($requestParts['jira_path'] . '/user', $requestParts['options']);

  if (!_ereolen_support_result_confirmed($result)) {
    return NULL;
  }

  $userCreated = json_decode($result->data);
  if (!empty($userCreated)) {
    return $userCreated;
  }

  return NULL;
}

/**
 * Set a Jira user to be active.
 *
 * @param \stdClass $jiraUser
 *   A Jira user.
 * @param array $form_state
 *   The state of the form.
 *
 * @return mixed
 *   A Jira user with status active.
 *
 * @throws \Exception
 */
function _ereolen_support_change_jira_user_status($jiraUser, $form_state) {
  $data = [
    'active' => true,
  ];
  $requestParts = _ereolen_support_get_request_parts('PUT', $data);
  $result = drupal_http_request($requestParts['jira_path'] . '/user?username=' . $jiraUser->name, $requestParts['options']);
  if (_ereolen_support_result_confirmed($result)){
    $jiraUser = json_decode($result->data);
  }

  return $jiraUser;
}

/**
 * Create an issue in Jira.
 *
 * @param array $form
 *   The support form.
 * @param array $form_state
 *   The state of the support form.
 * @param \stdClass $jiraUser
 *   A user in Jira.
 *
 * @return object|null
 *   The result of a create issue call to Jira or Null if the call failed.
 *
 * @throws \Exception
 */
function _ereolen_support_send_to_jira($form, &$form_state, $jiraUser) {
  $data = [
    'fields' => [
      'project' => [
        'id' => variable_get('jira_project_id'),
      ],
      'issuetype' => [
        'id' => variable_get('jira_issue_type_id'),
      ],
      'summary' => substr(preg_replace('/\s+/', ' ', trim(check_plain($form_state['values']['ereolen_support_problem']))), 0, 20),
      'description' => $form_state['values']['ereolen_support_description'],
      'reporter' => [
        'name' => $jiraUser->emailAddress,
      ],
    ],
  ];

  foreach ($form as $key => $element) {
    if (is_array($element)) {
      // Look for form fields.
      if (strpos($key, 'ereolen_support') === 0) {
        // Load config related to that field.
        $jira_field_id = variable_get($key . '_jira_field');
        // Make sure the config is set and has proper type.
        if (!empty($jira_field_id) && is_numeric($jira_field_id)) {
          // Add custom field to data array.
          $data['fields']['customfield_' . $jira_field_id] = check_plain($form_state['values'][$key]);
          if ($element['#type'] == 'select') {
            // Use Label as output from select lists.
            $data['fields']['customfield_' . $jira_field_id] = check_plain($element['#options'][$form_state['values'][$key]]);

            // Strip '- Select -' values we don't want them in jira.
            if ($data['fields']['customfield_' . $jira_field_id] == t('- Select -')) {
              $data['fields']['customfield_' . $jira_field_id] = '';
            }
          }
          else {
            $data['fields']['customfield_' . $jira_field_id] = check_plain($form_state['values'][$key]);
          }
        }
      }
    }
  }

  // Set Jira customer request type.
  // https://confluence.atlassian.com/jirakb/how-to-set-request-type-when-creating-an-issue-via-rest-api-using-rest-api-2-issue-endpoint-938041214.html
  $data = _ereolen_support_set_request_type(
    $data,
    variable_get('jira_support_request_type_field_id'),
    variable_get('jira_support_request_type_field_value')
  );

  $requestParts = _ereolen_support_get_request_parts('POST', $data);

  $result = drupal_http_request($requestParts['jira_path'] . '/issue', $requestParts['options']);

  if (_ereolen_support_result_confirmed($result)) {
    return $result;
  }

  return NULL;
}

/**
 * Create the parts required to make a drupal_http_request.
 *
 * @param string $method_type
 *   The request method.
 * @param array $data
 *   An optional data array.
 *
 * @return array
 *   An array containing base path for the request and the options related to
 *   the request.
 *
 * @return array
 */
  function _ereolen_support_get_request_parts($method_type, $data = [], $api = NULL) {
  $jiraUsername = variable_get('jira_username');
  $jiraPassword = variable_get('jira_password');
  // Jira requires base64_encode auth.
  $encoded_auth = base64_encode($jiraUsername . ':' . $jiraPassword);
  if ($api == 'servicedesk') {
    $jira_api_path = variable_get('jira_domain') . '/rest/servicedeskapi';
  }
  else {
    $jira_api_path = variable_get('jira_domain') . '/rest/api/latest';
  }

  $options = [
    'method' => $method_type,
    'timeout' => 15, // Seconds.
    'headers' => [
      'Content-Type' => 'application/json',
      'Authorization' => 'Basic '. $encoded_auth
    ],
  ];

  // Add data variable to POST and PUT requests.
  if ($method_type == 'POST' || $method_type == 'PUT' ) {
    $options['data'] = json_encode($data);
  }

  $requestParts = [
    'jira_path' => $jira_api_path,
    'options' => $options
  ];

  return $requestParts;
}

/**
 * Prepare an array for use in a select list.
 *
 * @param string $vocab_machine_name
 *  A vocabulary machine name.
 * @param bool $add_select
 *  If a - Select - option should be made available.
 *
 * @return array
 *   A list of term names.
 */
function _ereolen_support_create_select_list($vocab_machine_name, $add_select = TRUE) {
  $vacabulary = taxonomy_vocabulary_machine_name_load($vocab_machine_name);
  $terms = taxonomy_get_tree($vacabulary->vid);
  if ($terms) {
    if ($add_select) {
      $list['_none'] = t('- Select -');
    } else {
      $list = [];
    }

    foreach ($terms as $term) {
      $list[$term->name] = str_repeat('-', $term->depth) . $term->name;
    }

    return $list;
  }
  return [];
}

/**
 * Set array keys as values for use in select list.
 *
 * @param $array
 *  An array of values.
 * @return array
 *   An array with keys like values.
 */
function _ereolen_support_array_as_select($array) {
  $list = [];
  foreach ($array as $value) {
    $list[$value] = $value;
  }

  return $list;
}

/**
 * Provide a support ID grabbed from Drupal user.
 *
 * @return string|null
 *   If support id is found in session the id is returned else NULL
 */
function _ereolen_support_get_support_id() {
  if (user_is_logged_in() && isset($_SESSION['ding_user']['user']['data'])) {
    $user_data = $_SESSION['ding_user']['user']['data'];
    if (isset($user_data['support_id'])) {
      return $user_data['support_id'];
    }
  }
  return NULL;
}

/**
 * Add an entry to the eReolen support error log.
 * We use a separate error log for easier access to potential errors. This also
 * allows for future development of tools for supporters to locate errors.
 *
 * @param \stdClass $result
 *   The result of a jira http request.
 *
 * @return bool
 *   Whether the result code implies success or failure.
 *
 * @throws \Exception
 */
function _ereolen_support_result_confirmed($result) {
  if ($result->code !== '200' && $result->code !== '201') {
    $id = db_insert('ereolen_support_errors')
      ->fields([
        'timestamp' => time(),
        'request' => $result->request,
        'data' => serialize($result->data),
      ])->execute();

    return FALSE;
  }

  return TRUE;
}

/**
 * Set a Jira customer request type.
 *
 * @param array $data
 *   An array of submitted data about to be sent to Jira.
 * @param $customfield_id
 *   The id of the Customer request type custom field.
 * @param $request_type_value
 *   The value of the request type format (project_key/requesttype_key) fetched
 *   from db table "AO_54307E_VIEWPORTFORM", db column "KEY".
 *   https://confluence.atlassian.com/jirakb/how-to-set-request-type-when-creating-an-issue-via-rest-api-using-rest-api-2-issue-endpoint-938041214.html
 *
 * @return mixed
 *   An array of submitted data with requesttype.
 */
function _ereolen_support_set_request_type($data, $customfield_id, $request_type_value) {
  $data['fields']['customfield_' . $customfield_id] = $request_type_value;
  return $data;
}

/**
 * Validate the required fields.
 *
 * Since form_states only validate frontend we need back end validation for the
 * fields that are set as required with form_states.
 *
 * @param $element
 *  The form element to check validation against.
 *
 * @param $form_state
 *   The state of the form.
 */
function _ereolen_support_states_require_validate($element, $form_state) {
  $required_field_key = key($element['#states']['required']);
  $required_field = explode('"', $required_field_key);
  foreach ($element['#states']['required'][$required_field_key] as $field) {
    if ($form_state['values'][$required_field[1]] == $field['value']) {
      if ($form_state['values'][$element['#name']] == '' || $form_state['values'][$element['#name']] == '_none') {
        form_set_error($element['#name'], $element['#title'] . ' is required.');
      }
    }
  }
}

/**
 * Validate support id against exactly 5 characters
 *
 * @param $element
 *  The form element to check validation against.
 *
 * @param $form_state
 *   The state of the form.
 */
function _ereolen_support_support_id_validate($element, $form_state) {
  $visible_field_key = key($element['#states']['visible']);
    foreach ($element['#states']['visible'][$visible_field_key] as $field) {
    if ($form_state['values']['ereolen_support_problem'] == $field['value']) {
      if ($element['#value'] != '') {
        // Assumes php mbstring extension enabled.
        if (drupal_strlen($element['#value']) != 6) {
          form_set_error($element['#name'], $element['#title'].' should be 6 characters');
        }
      }
    }
  }
}

/**
 * Get library related to logged in user.
 *
 * @return string
 *   The name of the library.
 */
function _ereolen_support_get_library() {
  if (user_is_logged_in()) {
    return publizon_get_library($_SESSION['ding_user']['user']['data']['retailer_id'])->library_name;
  }
  return '_none';
}

/**
 * Get email related to logged in user.
 *
 * @return string
 *   The user email.
 */
function _ereolen_support_get_email() {
  if (user_is_logged_in()) {
    $profile = profile2_load_by_user($GLOBALS['user']->uid, 'provider_publizon');

    if ($profile) {
      $profile_wrapper = entity_metadata_wrapper('profile2', $profile);
      $email = $profile_wrapper->field_email->value();
      if (!empty($email)) {
        return $email;
      }
    }
  }
  return '';
}

/**
 * Delete support error log entry after 30 days.
 */
function _ereolen_support_cleanup_errors() {
  // 30 days.
  $month_in_seconds = 2592000;
  $deleted = db_delete('ereolen_support_errors')
    ->condition('timestamp', time() - $month_in_seconds, '<')
    ->execute();
}
