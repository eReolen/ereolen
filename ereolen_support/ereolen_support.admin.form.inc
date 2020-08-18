<?php

/**
 * @file
 * Add eReolen support admin form.
 */

/**
 * Create backend admin form.
 *
 * @param array $form
 *   The form to create.
 * @param array $form_state
 *   The state of the form.
 *
 * @return mixed
 */
function ereolen_support_config_form($form, &$form_state) {
  // The types of field we allow building through a loop later on.
  $fieldTypesBuild = ['select', 'textfield', 'textarea', 'emailfield', 'date_popup'];

  // The form array used in frontend to which we create form groups later on.
  $frontendForm = drupal_get_form('ereolen_support_form');

  $form['support_form'] = [
    '#type' => 'vertical_tabs',
  ];

  $form['jira_settings'] = [
    '#type' => 'fieldset',
    '#title' => t('Jira settings'),
    '#group' => 'support_form',
    '#collapsible' => FALSE,
    '#access'=> user_access('administer ereolen support form')
  ];

  $form['field_config'] = [
    '#type' => 'fieldset',
    '#title' => t('Field config'),
    '#group' => 'support_form',
    '#collapsible' => FALSE,
  ];

  // The domain of a jira installation.
  $form['jira_settings']['jira_domain'] = [
    '#type' => 'textfield',
    '#title' => t('Jira domain'),
    '#default_value' => variable_get('jira_domain'),
  ];

  // A Jira username.
  $form['jira_settings']['jira_username'] = [
    '#type' => 'textfield',
    '#title' => t('Jira username'),
    '#default_value' => variable_get('jira_username'),
  ];

  // The jira users password.
  $form['jira_settings']['jira_password'] = [
    '#type' => 'password',
    '#title' => t('Jira user password'),
    '#default_value' => variable_get('jira_password'),
  ];

  // The jira project id fetched from a jira project.
  $form['jira_settings']['jira_project_id'] = [
    '#type' => 'textfield',
    '#title' => t('Jira project id'),
    '#default_value' => variable_get('jira_project_id'),
  ];

  // The desired issue type id fetched from a jira project
  $form['jira_settings']['jira_issue_type_id'] = [
    '#type' => 'textfield',
    '#title' => t('Jira issue type id'),
    '#default_value' => variable_get('jira_issue_type_id'),
  ];

  // Build field configuration from frontend form.
  foreach ($frontendForm as $key => $possibleField) {
    // Make sure the form entry is an array.
    if(is_array($possibleField)) {
      // Make sure it's a field type.
      if(in_array('#type', $possibleField) && in_array($possibleField['#type'], $fieldTypesBuild)) {
        // Build fieldset
        $form['field_config'][$possibleField['#name'] . '_group'] = [
          '#type' => 'fieldset',
          '#title' => $possibleField['#title'] . ' field',
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
        ];

        // Build form field description.
        $form['field_config'][$possibleField['#name'] . '_group'][$possibleField['#name'] . '_description'] = [
          '#type' => 'textfield',
          '#title' => $possibleField['#title'] . ' field help text',
          '#default_value' => variable_get($possibleField['#name'] . '_description'),
          '#description' => t('Set a help text for the field in frontend')
        ];

        // Build form field required.
        $form['field_config'][$possibleField['#name'] . '_group'][$possibleField['#name'] . '_required'] = [
          '#type' => 'checkbox',
          '#title' => t('This field is required'),
          '#default_value' => variable_get($possibleField['#name'] . '_required'),
          '#description' => t('By checking this you make the field required i frontend.')
        ];

        // Build form field jira field
        $form['field_config'][$possibleField['#name'] . '_group'][$possibleField['#name'] . '_jira_field'] = [
          '#type' => 'textfield',
          '#title' => 'Jira field id',
          '#default_value' => variable_get($possibleField['#name'] . '_jira_field'),
          '#access'=> user_access('administer ereolen support form'),
          '#description' => t('A numeric value i.e 12345. Use this field to map eReolen fields to Jira fields.'),
        ];
      }
    }
  }

  $form['publizon'] = [
    '#type' => 'fieldset',
    '#title' => t('Publizon settings'),
    '#group' => 'support_form',
    '#collapsible' => FALSE,
    '#access'=> user_access('administer ereolen support form')
  ];

  $form['publizon']['publizon_support_email'] = [
    '#type' => 'emailfield',
    '#title' => t('Publizon support email'),
    '#default_value' => variable_get('publizon_support_email'),
  ];

  return system_settings_form($form);
}
