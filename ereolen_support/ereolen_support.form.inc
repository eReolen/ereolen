<?php

/**
 * @file
 * Add eReolen support form.
 */

include_once 'ereolen_support.helper.inc';

/**
 * Create frontend form.
 *
 * @param array $form
 *   The form to create.
 * @param array $form_state
 *   The state of the form.
 *
 * @return mixed
 */
function ereolen_support_form($form, &$form_state) {
  global $user;
  $form['#attributes'] = array(
    'name' => 'ereolen_support_form',
    'class' => array('node-webform')
  );
  $form['#attached']['js'] = array(
    drupal_get_path('module', 'ereolen_support') . '/js/collectDeviceData.js',
    array(
      'type' => 'setting',
      'data' => array(
        'ereolen_support' => array(
          'form_state' => $form_state,
        ),
      ),
    ),
  );

  // @todo libraries array.
  $libraries = ['a' => '1', 'b' => '2'];
  $form['ereolen_support_library'] = array(
    '#type' => 'select',
    '#title' => t('Library'),
    '#options' => $libraries,
    '#description' => variable_get('ereolen_support_library_description'),
    '#required' => variable_get('ereolen_support_library_required'),
  );

  $form['ereolen_support_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => variable_get('ereolen_support_name_description'),
    '#required' => variable_get('ereolen_support_name_required')
  );

  $form['ereolen_support_email'] = array(
    '#type' => 'emailfield',
    '#title' => t('Email'),
    '#description' => variable_get('ereolen_support_email_description'),
    '#required' => variable_get('ereolen_support_email_required')
  );

  $form['ereolen_support_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Support ID'),
    '#description' => variable_get('ereolen_support_id_description'),
    '#required' => variable_get('ereolen_support_id_required'),
    '#default_value' => _ereolen_support_get_support_id($user)
  );

  $form['ereolen_support_product'] = array(
    '#type' => 'select',
    '#title' => t('Product'),
    '#options' => _ereolen_support_create_select_list('ereolen_support_product'),
    '#description' => variable_get('ereolen_support_product_description'),
    '#required' => variable_get('ereolen_support_product_description_required')
  );

  $form['ereolen_support_problem'] = array(
    '#type' => 'select',
    '#title' => t('Problem'),
    '#options' => _ereolen_support_create_select_list('ereolen_support_problem'),
    '#description' => variable_get('ereolen_support_problem_description'),
    '#required' => variable_get('ereolen_support_problem_required')
  );

  $form['ereolen_support_description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => variable_get('ereolen_support_description_description'),
    '#required' => variable_get('ereolen_support_description_required')
  );

  $form['ereolen_support_date'] = array(
    '#type' => 'date_popup',
    '#title' => t('Dato'),
    '#date_format' => 'd/m/Y',
    '#date_label_position' => 'none',
    '#description' => variable_get('ereolen_support_date_description'),
    '#required' => variable_get('ereolen_support_date_required')
  );

  $form['ereolen_support_book_title'] = array(
    '#type' => 'textfield',
    '#title' => t('What book does the problem concern?'),
    '#description' => variable_get('ereolen_support_book_title_description'),
    '#required' => variable_get('ereolen_support_book_title_required')
  );

  $form['ereolen_support_device'] = array(
    '#type' => 'select',
    '#title' => t('What device did you use?'),
    '#options' => _ereolen_support_create_select_list('ereolen_support_device'),
    '#description' => variable_get('ereolen_support_device_description'),
    '#required' => variable_get('ereolen_support_device_required')
  );

  $form['ereolen_support_model'] = array(
    '#type' => 'textfield',
    '#title' => t('On what device model did you experience the problem?'),
    '#description' => variable_get('ereolen_support_model_description'),
    '#required' => variable_get('ereolen_support_model_required')
  );

  $form['ereolen_support_operating_system'] = array(
    '#type' => 'textfield',
    '#title' => t('On what operating system did you experience the problem?'),
    '#description' => variable_get('ereolen_support_operating_system_description'),
    '#required' => variable_get('ereolen_support_model_operating_system')
  );

  $form['submit_button'] = array(
    '#type' => 'submit',
    '#value' => t('Send'),
  );

  return $form;
}

/**
 * Add a submit handler to the support form.
 *
 * @param $form
 *   The form to submit.
 * @param $form_state
 *   The state of the form.
 *
 * @throws \Exception
 */
function ereolen_support_form_submit($form, &$form_state) {
  global $user;
  // Check for and create user in Jira
  $jiraUser = _ereolen_support_search_jira_user($user, $form_state);
  if (!$jiraUser) {
    $jiraUser = _ereolen_support_create_jira_user($user, $form_state);
  }

  // Create issue in Jira.
  if (!empty($jiraUser)) {
    _ereolen_support_send_to_jira($form, $form_state, $jiraUser);
  }



}



