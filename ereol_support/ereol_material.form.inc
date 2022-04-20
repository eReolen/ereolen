<?php

/**
 * @file
 * Add eReolen material selection form.
 */

include_once 'ereol_support.helper.inc';

/**
 * Create material selection form.
 *
 * @param array $form
 *   The form to create.
 * @param array $form_state
 *   The state of the form.
 *
 * @return mixed
 *
 * @throws \Exception
 */
function ereol_material_form($form, &$form_state) {
  $form['#attributes'] = [
    'name' => 'ereol_material_form',
    'class' => ['node-webform']
  ];

  $form['#attached']['css'] = [
    drupal_get_path('module', 'ereol_support') . '/css/styles.css'
  ];

  $form['ereol_material_lead'] = [
    '#markup' => theme('ereol_support_lead_text', [
      'text' =>  variable_get('ereol_material_lead')['value'] ?? NULL,
    ]),
  ];

  $form['ereol_material_email'] = [
    '#type' => 'emailfield',
    '#title' => t('Email'),
    '#required' => TRUE,
  ];

  $form['ereol_material_description'] = [
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#required' => TRUE,
  ];

  $form['submit_button'] = [
    '#type' => 'submit',
    '#value' => t('Send'),
  ];

  honeypot_add_form_protection($form, $form_state, ['honeypot', 'time_restriction']);

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
function ereol_material_form_submit($form, &$form_state) {
  global $user;
  $submission_text = variable_get('ereol_material_submission_text');

  // Check for and create user in Jira
  $jira_user = _ereol_support_search_jira_user($user, $form_state, $form_state['values']['ereol_material_email']);
  if (!$jira_user) {
    $jira_user = _ereol_support_create_jira_user($user, $form_state, $form_state['values']['ereol_material_email']);
  }

  // Create issue in Jira.
  if (!empty($jira_user)) {
    $jira_result = _ereol_material_send_to_jira($form, $form_state, $jira_user);
    if (!empty($jira_result)) {
      drupal_set_message($submission_text, 'status');
    }
  }
}
