<?php

/**
 * @file
 * Contains helper functions for the support form.
 */

/**
 * Search for a user.
 *
 * @param string $email
 *   Email of the user.
 *
 * @return string|null
 *   Account id of the user.
 */
function _ereolen_support_search_user(string $email): ?string {
  $searchQueryResult = _ereolen_support_call_api('/user/search?' . http_build_query(['query' => $email]), 'GET');
  // Endpoint searches in all fields, so we must ensure email matches.
  $content = $searchQueryResult['content'] ?? NULL;
  if (is_array($content)) {
    foreach ($content as $match) {
      // Atlassian accounts have their email stripped from user results,
      // so in that case we have no email to match up against.
      if ($match->emailAddress === $email || (empty($match->emailAddress) && $match->accountType === 'atlassian')) {
        return $match->accountId;
      }
    }
  }

  return NULL;
}

/**
 * Create a customer.
 *
 * @param string $email
 *   Email of the customer to create.
 *
 * @return array|null
 *   The result of the request.
 */
function _ereolen_support_create_customer(string $email): ?array {
  return _ereolen_support_call_api(
    '/customer?' . http_build_query(['strictConflictStatusCode' => 'true']),
    'POST',
    [
      'displayName' => $email,
      'email' => $email,
    ],
    'servicedesk'
  );
}

/**
 * Create an issue in Jira.
 *
 * @param array $form
 *   The support form.
 * @param array $form_state
 *   The state of the support form.
 * @param string $jiraUserId
 *   A user in Jira.
 *
 * @return array|null
 *   The result of a create issue call to Jira or Null if the call failed.
 *
 * @throws \Exception
 */
function _ereolen_support_send_to_jira(array $form, array $form_state, string $jiraUserId): ?array {
  // 'description" must not be blank in Jira.
  $description = trim($form_state['values']['ereolen_support_description']);
  if (empty($description)) {
    $description = '.';
  }

  $data = [
    'isAdfRequest' => FALSE,
    'requestFieldValues' => [
      'description' => $description,
    ],
    'raiseOnBehalfOf' => $jiraUserId,
    'serviceDeskId' => variable_get('jira_support_service_desk_id'),
  ];

  unset($form['ereolen_support_problem']);

  foreach ($form as $key => $element) {
    if (is_array($element)) {
      // Look for form fields.
      if (strpos($key, 'ereolen_support') === 0) {
        if (!_ereolen_support_element_is_visible($element, $form_state)) {
          continue;
        }
        // Load config related to that field.
        $jira_field_id = variable_get($key . '_jira_field');
        // Make sure the config is set and has proper type.
        if (!empty($jira_field_id) && is_numeric($jira_field_id)) {
          // Add custom field to data array.
          if ($form_state['values'][$key] !== '_none' && !empty($form_state['values'][$key])) {
            $data['requestFieldValues']['customfield_' . $jira_field_id] = check_plain($form_state['values'][$key]);
            if ($element['#type'] == 'select') {
              // If library field treat select as free text to match Jira.
              if ($jira_field_id === variable_get('ereolen_support_library_jira_field')) {
                $data['requestFieldValues']['customfield_' . $jira_field_id] = check_plain($element['#options'][$form_state['values'][$key]]);
              }
              else {
                // Use Label as output from select lists.
                $data['requestFieldValues']['customfield_' . $jira_field_id] = ['value' => check_plain($element['#options'][$form_state['values'][$key]])];
              }
            }
            else {
              $data['requestFieldValues']['customfield_' . $jira_field_id] = check_plain($form_state['values'][$key]);
            }
          }
        }
      }
    }
  }

  $data['requestTypeId'] = _ereolen_get_problem_id_from_string($form_state['values']['ereolen_support_problem']);

  return _ereolen_support_call_api('/request', 'POST', $data, 'servicedesk');
}

/**
 * Send material service desk request to Jira.
 *
 * @param array $form_state
 *   State of the form.
 * @param string $jiraUserId
 *   Jira user accountId.
 *
 * @return array|null
 *   The result of the request.
 *
 * @throws \Exception
 */
function _ereolen_material_send_to_jira(array $form_state, string $jiraUserId): ?array {
  $data = [
    'isAdfRequest' => FALSE,
    'requestFieldValues' => [
      'summary' => $form_state['values']['ereolen_material_type'],
      'description' => $form_state['values']['ereolen_material_description'],
    ],
    'raiseOnBehalfOf' => $jiraUserId,
    'serviceDeskId' => variable_get('jira_material_service_desk_id'),
    'requestTypeId' => variable_get('jira_material_service_desk_request_type_id'),
  ];

  return _ereolen_support_call_api('/request', 'POST', $data, 'servicedesk');
}

/**
 * Group all parts needed to make a request into an array.
 *
 * @param string $apiEndpoint
 *   The api endpoint.
 * @param string $method_type
 *   The method to be used.
 * @param array $data
 *   Data for post request methods.
 * @param string|null $api
 *   Api to use. Service desk or jira core.
 *
 * @return array
 *   An array containing mehod, path, data and auth for the request.
 *   the request.
 */
function _ereolen_support_get_request_parts(string $apiEndpoint, string $method_type, array $data = [], string $api = NULL): array {
  $jira_api_path = variable_get('jira_domain')
    . ('servicedesk' === $api ? '/rest/servicedeskapi' : '/rest/api/latest');

  return [
    'method' => $method_type,
    'base_url' => $jira_api_path,
    'endpoint' => $apiEndpoint,
    'auth' => [
      'user_name' => variable_get('jira_username'),
      'token' => variable_get('jira_token'),
    ],
    'data' => $data,
  ];
}

/**
 * Prepare an array for use in a select list.
 *
 * @param string $vocab_machine_name
 *   A vocabulary machine name.
 * @param bool $add_select
 *   If a - Select - option should be made available.
 *
 * @return array
 *   A list of term names.
 */
function _ereolen_support_create_select_list(string $vocab_machine_name, bool $add_select = TRUE): array {
  $vacabulary = taxonomy_vocabulary_machine_name_load($vocab_machine_name);
  $terms = taxonomy_get_tree($vacabulary->vid);
  if ($terms) {
    if ($add_select) {
      $list['_none'] = t('- Select -');
    }
    else {
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
 * @param array $array
 *   An array of values.
 *
 * @return array
 *   An array with keys like values.
 */
function _ereolen_support_array_as_select(array $array, $add_select = TRUE): array {
  $list = [];
  if ($add_select) {
    $list['_none'] = t('- Select -');
  }

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
function _ereolen_support_get_support_id(): ?string {
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
 *
 * We use a separate error log for easier access to potential errors. This also
 * allows for future development of tools for supporters to locate errors.
 *
 * <code>
 * SELECT FROM_UNIXTIME(timestamp), JSON_DETAILED(request), JSON_DETAILED(data)
 *   FROM ereolen_support_errors\G
 * </code>
 *
 * @param array $result
 *   The result of a jira http request.
 *
 * @return bool
 *   Whether the result code implies success or failure.
 *
 * @throws \Exception
 */
function _ereolen_support_result_confirmed(array $result): bool {
  if ($result['info']['error_code'] !== 200 && $result['info']['error_code'] !== 201) {
    $id = db_insert('ereolen_support_errors')
      ->fields(['timestamp', 'request', 'data'])
      ->values([
        time(),
        json_encode($result['info']['request_parts'] ?? NULL),
        json_encode($result),
      ])->execute();

    return FALSE;
  }

  return TRUE;
}

/**
 * Validate the required fields.
 *
 * Since form_states only validate frontend we need back end validation for the
 * fields that are set as required with form_states.
 *
 * @param array $element
 *   The form element to check validation against.
 * @param array $form_state
 *   The state of the form.
 */
function _ereolen_support_states_require_validate(array $element, array $form_state) {
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
 * Decide if an element is visible based on states.
 *
 * NOTE: This is a very limited implementation basically handling only the
 * support form case, i.e.
 *
 * <code>
 *  '#states' => [
 *    'visible' => [
 *      ':input[name="ereolen_support_problem"]' => [
 *         ['value' => '…'],
 *         ['value' => '…'],
 *         …
 *       ],
 *     ],
 *   ],
 * </code>
 *
 * @param array $element
 *   The form element to check visibility of.
 * @param array $form_state
 *   The state of the form.
 */
function _ereolen_support_element_is_visible(array $element, array $form_state): bool {
  if (isset($element['#states']['visible'])) {
    foreach ($element['#states']['visible'] as $selector => $spec) {
      if (preg_match('/^:input\[name="(?P<name>[^"]+)"]/', $selector, $matches)) {
        $value = $form_state['values'][$matches['name']] ?? NULL;
        if ($value && is_array($spec)) {
          $required_values = array_column($spec, 'value');

          return in_array($value, $required_values);
        }
      }
    }
  }

  return TRUE;
}

/**
 * Validate support id against exactly 5 characters.
 *
 * @param array $element
 *   The form element to check validation against.
 * @param array $form_state
 *   The state of the form.
 */
function _ereolen_support_support_id_validate(array $element, array $form_state) {
  $visible_field_key = key($element['#states']['visible']);
  foreach ($element['#states']['visible'][$visible_field_key] as $field) {
    if ($form_state['values']['ereolen_support_problem'] == $field['value']) {
      if ($element['#value'] != '') {
        // Assumes php mbstring extension enabled.
        if (drupal_strlen($element['#value']) != 6) {
          form_set_error($element['#name'], $element['#title'] . ' should be 6 characters');
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
function _ereolen_support_get_library(): string {
  if (user_is_logged_in()
    && $retailerId = ($_SESSION['ding_user']['user']['data']['retailer_id'] ?? NULL)) {
    $library = publizon_get_library($retailerId);
    if ($library instanceof PublizonConfiguredLibrary) {
      return $library->library_name;
    }
  }
  return '_none';
}

/**
 * Get the id of support problem from the problem string.
 *
 * @param string $problem
 *   The problem as presented as string.
 *
 * @return mixed
 *   The id related to that problem.
 */
function _ereolen_get_problem_id_from_string(string $problem) {
  $problems = variable_get('ereolen_support_problems');

  return $problems[$problem];
}

/**
 * Get email related to logged in user.
 *
 * @return string
 *   The user email.
 *
 * @throws \EntityMetadataWrapperException
 */
function _ereolen_support_get_email(): string {
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

/**
 * Curl setup for requesting Jira API.
 *
 * @param string $apiEndpoint
 *   The api endpoint.
 * @param string $method_type
 *   The method type.
 * @param array $data
 *   The data to add to the request.
 * @param string|null $api
 *   The api endpoint to use.
 *
 * @return array|null
 *   The relevant parts of the result of the request wrapped in an array.
 */
function _ereolen_support_call_api(string $apiEndpoint, string $method_type, array $data = [], string $api = NULL): ?array {
  $requestParts = _ereolen_support_get_request_parts($apiEndpoint, $method_type, $data, $api);

  $headers = [
    'Accept: application/json',
    'Content-Type: application/json',
  ];

  // Initialize curl.
  $curl = curl_init();

  // Set method specific options.
  switch ($requestParts['method']) {
    case 'POST':
      curl_setopt($curl, CURLOPT_POST, 1);

      if ($requestParts['data']) {
        curl_setopt($curl, CURLOPT_POSTFIELDS, json_encode($requestParts['data']));
      }
      break;

    case 'PUT':
      curl_setopt($curl, CURLOPT_CUSTOMREQUEST, 'PUT');

      if ($requestParts['data']) {
        curl_setopt($curl, CURLOPT_POSTFIELDS, json_encode($requestParts['data']));
      }
      break;

    default:
  }

  // Set global options.
  curl_setopt_array($curl, [
    CURLOPT_URL => $requestParts['base_url'] . $requestParts['endpoint'],
    CURLOPT_RETURNTRANSFER => TRUE,
    CURLOPT_HTTPAUTH => CURLAUTH_BASIC,
    CURLOPT_FOLLOWLOCATION => TRUE,
    CURLOPT_HTTPHEADER => $headers,
    CURLOPT_USERPWD => $requestParts['auth']['user_name'] . ':' . $requestParts['auth']['token'],
    CURLOPT_CONNECTTIMEOUT => 0,
    CURLOPT_TIMEOUT => 15,
  ]);

  $result = curl_exec($curl);

  $output = [
    'content' => json_decode($result),
    'info' => [
      'error_code' => curl_getinfo($curl, CURLINFO_HTTP_CODE),
      'error' => curl_errno($curl) ? curl_error($curl) : '',
      'request_parts' => $requestParts,
    ],
  ];

  curl_close($curl);

  if (!_ereolen_support_result_confirmed($output)) {
    return NULL;
  }

  return $output;
}
