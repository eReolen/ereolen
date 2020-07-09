<?php

/**
 * @file
 * Contains helper functions for the support form.
 */

/**
 * Search for a drupal user email in Jira.
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
  $requestParts = _ereolen_support_get_request_parts('GET');
  $result = drupal_http_request($requestParts['jira_path'] . '/user/search?includeInactive=true&username=' . $user->mail, $requestParts['options']);
  $resultArray = json_decode($result->data);
  _ereolen_support_log_error($result);

  // Searches for matches in full name, username and email, so we may have
  // multiple results.
  if(!empty($resultArray)) {
    foreach ($resultArray as $jiraUser) {
      if($jiraUser->name == $user->mail) {
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
  $data = [
    'name' => $user->mail,
    'emailAddress' => $user->mail,
    'displayName' => $user->mail,
  ];

  $requestParts = _ereolen_support_get_request_parts('POST', $data);

  $result = drupal_http_request($requestParts['jira_path'] . '/user', $requestParts['options']);
  _ereolen_support_log_error($result);
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
  _ereolen_support_log_error($result);
  $jiraUser = json_decode($result->data);

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
      'summary' => $form_state['values']['ereolen_support_description'],
      'description' => $form_state['values']['ereolen_support_description'],
      'reporter' => [
        'name' => $jiraUser->emailAddress,
      ],
    ],
  ];
  foreach ($form as $key => $element) {
    if(is_array($element)) {
      // Look for form fields.
      if(strpos($key, 'ereolen_support') === 0) {
        // Load config related to that field.
        $jira_field_id = variable_get($key . '_jira_field');
        // Make sure the config is set and has proper type.
        if (!empty($jira_field_id) && is_numeric($jira_field_id)) {
          // Add custom field to data array.
          $data['fields']['customfield_' . $jira_field_id] = $form_state['values'][$key];
          if($element['#type'] == 'select') {
            // Use Label as output from select lists.
            $data['fields']['customfield_' . $jira_field_id] = $element['#options'][$form_state['values'][$key]];
          }
          else {
            $data['fields']['customfield_' . $jira_field_id] = $form_state['values'][$key];
          }
        }
      }
    }
  }

  $requestParts = _ereolen_support_get_request_parts('POST', $data);

  $result = drupal_http_request($requestParts['jira_path'] . '/issue', $requestParts['options']);
  _ereolen_support_log_error($result);
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
function _ereolen_support_get_request_parts($method_type, $data = []) {
  $jiraUsername = variable_get('jira_username');
  $jiraPassword = variable_get('jira_password');
  // Jira requires base64_encode auth.
  $encoded_auth = base64_encode($jiraUsername . ':' . $jiraPassword);
  $jira_api_path = variable_get('jira_domain') . '/rest/api/latest';

  $options = array(
    'method' => $method_type,
    'timeout' => 15,
    'headers' => array(
      'Content-Type' => 'application/json',
      'Authorization' => 'Basic '. $encoded_auth
    ),
  );

  // Add data variable to POST and PUT requests.
  if ($method_type == 'POST' || $method_type == 'PUT' ) {
    $options['data'] = json_encode($data);
  }

  $requestParts = array(
    'jira_path' => $jira_api_path,
    'options' => $options
  );

  return $requestParts;
}

/**
 * Prepare an array for use in a select list.
 *
 * @param string $vocab_machine_name
 *  A vocabulary machine name.
 * @return array
 *   A list of term names.
 */
function _ereolen_support_create_select_list($vocab_machine_name) {
  $vacabulary = taxonomy_vocabulary_machine_name_load($vocab_machine_name);
  $terms = taxonomy_get_tree($vacabulary->vid);
  if ($terms) {
    $list = [];
    foreach ($terms as $term) {
      $list[$term->tid] = str_repeat('-', $term->depth) . $term->name;
    }

    return $list;
  }
  return [];
}

/**
 * Provide a support ID grabbed from Drupal user.
 *
 * @param \stdClass $user
 *   A drupal user.
 * @return null
 */
function _ereolen_support_get_support_id($user) {
  // @todo If user is logged in provide a Support id.
  return NULL;
}

/**
 * Add an entry to the eReolen support error log.
 *
 * @param \stdClass $result
 *   The result of a jira http request.
 *
 * @throws \Exception
 */
function _ereolen_support_log_error($result) {
  if ($result->code !== '200' && $result->code !== '201') {
    $id = db_insert('ereolen_support_errors')
      ->fields(array(
        'timestamp' => format_date(time()),
        'request' => $result->request,
        'data' => serialize($result->data),
      ))->execute();
    drupal_set_message(t('An error occurred. Your support issue was not registered.'), 'error');
  }
  else {
    drupal_set_message(t('The form has been submitted.'));
  }
}
