<?php

/**
 * @file
 * Add eReolen material selection form.
 */

include_once 'ereolen_support.helper.inc';

/**
 * Create material selection form.
 *
 * @param array $form
 *   The form to create.
 * @param array $form_state
 *   The state of the form.
 *
 * @return mixed
 *   Material form array.
 *
 * @throws \Exception
 */
function ereolen_material_form(array $form, array &$form_state) {
  $form['#attributes'] = [
    'name' => 'ereolen_material_form',
    'class' => ['node-webform'],
  ];

  $form['#attached']['css'] = [
    drupal_get_path('module', 'ereolen_support') . '/css/styles.css',
  ];

  $form['ereolen_material_lead'] = [
    '#markup' => theme('ereolen_support_lead_text', [
      'text' => variable_get('ereolen_material_lead')['value'] ?? NULL,
    ]),
  ];

  $form['ereolen_material_email'] = [
    '#type' => 'emailfield',
    '#title' => t('Email'),
    '#required' => TRUE,
  ];

  $form['ereolen_material_type'] = [
    '#type' => 'select',
    '#title' => t('Type'),
    '#options' => [
      t('E-book') => t('E-book'),
      t('Audiobook') => t('Audiobook'),
      t('E-book and audiobook') => t('E-book and audiobook'),
      t('Podcast') => t('Podcast'),
      t('Andet') => t('Andet'),
    ],
    '#required' => TRUE,
  ];

  $form['ereolen_material_description'] = [
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#required' => TRUE,
  ];

  $form['captcha'] = array(
    '#type' => 'captcha',
    '#captcha_type' => 'image_captcha/Image',
  );

  $form['submit_button'] = [
    '#type' => 'submit',
    '#value' => t('Send'),
  ];

  honeypot_add_form_protection(
    $form,
    $form_state,
    ['honeypot', 'time_restriction']
  );

  return $form;
}

/**
 * Add a submit handler to the support form.
 *
 * @param array $form
 *   The form to submit.
 * @param array $form_state
 *   The state of the form.
 *
 * @throws \Exception
 */
function ereolen_material_form_submit(array $form, array &$form_state) {
  $submission_text = variable_get('ereolen_material_submission_text');

  $email = $form_state['values']['ereolen_material_email'];
  // Search for a user.
  $jiraUserId = _ereolen_support_search_user($email);

  // Create user if none are found.
  if (empty($jiraUserId)) {
    try {
      $jiraUser = _ereolen_support_create_customer($email);
      if (empty($jiraUser)) {
        throw new RuntimeException('Error creating Jira user');
      }
      $jiraUserId = $jiraUser['content']->accountId;
    }
    catch (Exception $e) {
      watchdog('ereolen_support', $e);
      drupal_set_message(t('Submission failed'));
      return;
    }
  }

  // Create issue in Jira.
  $jira_result = _ereolen_material_send_to_jira($form_state, $jiraUserId);
  if (!empty($jira_result)) {
    drupal_set_message($submission_text);
  }
  else {
    drupal_set_message(t('Submission failed'));
  }
}
