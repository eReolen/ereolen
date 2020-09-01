<?php

/**
 * @file
 * Add eReolen support form.
 * @todo Add form validation to ensure no cpr numbers are added.
 */

include_once 'ereolen_support.helper.inc';
include_once 'ereolen_support.cpr-checker.inc';

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
  $form['#attributes'] = [
    'name' => 'ereolen_support_form',
    'class' => ['node-webform']
  ];

  $form['#attached']['css'] = [
    drupal_get_path('module', 'ereolen_support') . '/css/styles.css'
  ];
  $form['#attached']['js'] = [
    drupal_get_path('module', 'ereolen_support') . '/js/collectDeviceData.js',
    [
      'type' => 'setting',
      'data' => [
        'ereolen_support' => [
          'form_state' => $form_state,
        ],
      ],
    ],
    drupal_get_path('module', 'ereolen_support') . '/js/showHelpText.js',
    [
      'type' => 'setting'
    ],
  ];

  $form['ereolen_support_problem'] = [
    '#type' => 'select',
    '#title' => t('Problem'),
    '#options' => _ereolen_support_create_select_list('ereolen_support_problem'),
    '#description' => variable_get('ereolen_support_problem_description'),
    '#required' => variable_get('ereolen_support_problem_required')
  ];

  $vacabulary = taxonomy_vocabulary_machine_name_load('ereolen_support_problem');
  $problemValues = taxonomy_get_tree($vacabulary->vid);
  $form['ereolen_support_help_text'] = array(
    '#markup' => theme('ereolen_support_help_text', [
      'problem_values' => $problemValues,
    ]),
  );


  $libraries = module_exists('publizon') ? publizon_get_libraries_list() : [];
  $form['ereolen_support_library'] = [
    '#type' => 'select',
    '#title' => t('Library'),
    '#options' => _ereolen_support_array_as_select($libraries),
    '#description' => variable_get('ereolen_support_library_description'),
    '#required' => variable_get('ereolen_support_library_required'),
  ];

  $form['ereolen_support_name'] = [
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => variable_get('ereolen_support_name_description'),
    '#required' => variable_get('ereolen_support_name_required'),
    '#element_validate' => array('_ereolen_support_check_cpr'),
  ];

  $form['ereolen_support_email'] = [
    '#type' => 'emailfield',
    '#title' => t('Email'),
    '#description' => variable_get('ereolen_support_email_description'),
    '#required' => variable_get('ereolen_support_email_required')
  ];

  $form['ereolen_support_id'] = [
    '#type' => 'textfield',
    '#title' => t('Support ID'),
    '#description' => variable_get('ereolen_support_id_description'),
    '#required' => variable_get('ereolen_support_id_required'),
    '#default_value' => _ereolen_support_get_support_id(),
    '#element_validate' => array('_ereolen_support_check_cpr'),
  ];

  $form['ereolen_support_error_message'] = [
    '#type' => 'textfield',
    '#title' => t('Error message'),
    '#description' => variable_get('ereolen_support_error_message_description'),
    '#required' => variable_get('ereolen_support_error_message_required'),
    '#default_value' => variable_get('ereolen_support_error_message'),
    '#element_validate' => array('_ereolen_support_check_cpr'),
  ];

  $form['ereolen_support_login_method'] = [
    '#type' => 'textfield',
    '#title' => t('Login method'),
    '#description' => variable_get('ereolen_support_login_method_description'),
    '#required' => variable_get('ereolen_support_login_method_required'),
    '#default_value' => variable_get('ereolen_support_login_method'),
    '#element_validate' => array('_ereolen_support_check_cpr'),
  ];

  $form['ereolen_support_book_type'] = [
    '#type' => 'textfield',
    '#title' => t('Book type  '),
    '#description' => variable_get('ereolen_support_book_type_description'),
    '#required' => variable_get('ereolen_support_book_type_required'),
    '#default_value' => variable_get('ereolen_support_book_type'),
    '#element_validate' => array('_ereolen_support_check_cpr'),
  ];

  $form['ereolen_support_product'] = [
    '#type' => 'select',
    '#title' => t('Product'),
    '#options' => _ereolen_support_create_select_list('ereolen_support_product'),
    '#description' => variable_get('ereolen_support_product_description'),
    '#required' => variable_get('ereolen_support_product_description_required')
  ];

  $form['ereolen_support_description'] = [
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => variable_get('ereolen_support_description_description'),
    '#required' => variable_get('ereolen_support_description_required'),
    '#element_validate' => array('_ereolen_support_check_cpr'),
  ];

  $form['ereolen_support_date'] = [
    '#type' => 'date_popup',
    '#title' => t('Dato'),
    '#date_format' => 'd/m/Y',
    //'#date_format' => array_pop(system_get_date_formats('ding_material_lists_date'))['format'],
    '#date_label_position' => 'none',
    '#description' => variable_get('ereolen_support_date_description'),
    '#required' => variable_get('ereolen_support_date_required')
  ];

  $form['ereolen_support_book_title'] = [
    '#type' => 'textfield',
    '#title' => t('What book does the problem concern?'),
    '#description' => variable_get('ereolen_support_book_title_description'),
    '#required' => variable_get('ereolen_support_book_title_required'),
    '#element_validate' => array('_ereolen_support_check_cpr'),
  ];

  $form['ereolen_support_device'] = [
    '#type' => 'select',
    '#title' => t('What device did you use?'),
    '#options' => _ereolen_support_create_select_list('ereolen_support_device'),
    '#description' => variable_get('ereolen_support_device_description'),
    '#required' => variable_get('ereolen_support_device_required')
  ];

  $form['ereolen_support_model'] = [
    '#type' => 'textfield',
    '#title' => t('On what device model did you experience the problem?'),
    '#description' => variable_get('ereolen_support_model_description'),
    '#required' => variable_get('ereolen_support_model_required'),
    '#element_validate' => array('_ereolen_support_check_cpr'),
  ];

  $form['ereolen_support_operating_system'] = [
    '#type' => 'textfield',
    '#title' => t('On what operating system did you experience the problem?'),
    '#description' => variable_get('ereolen_support_operating_system_description'),
    '#required' => variable_get('ereolen_support_model_operating_system'),
    '#element_validate' => array('_ereolen_support_check_cpr'),
  ];

  $form['submit_button'] = [
    '#type' => 'submit',
    '#value' => t('Send'),
  ];

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
  $jira_user = _ereolen_support_search_jira_user($user, $form_state);
  if (!$jira_user) {
    $jira_user = _ereolen_support_create_jira_user($user, $form_state);
  }

  // @todo switch between jira and email This functionality is awaiting customer.

  // Sometimes send to publizon mail.
  $language = $user->uid ? user_preferred_language($user) : language_default();
  $headers = [
    'MIME-Version' => '1.0',
    'Content-Type' => 'text/html; charset=UTF-8; format=flowed; delsp=yes',
    'Content-Transfer-Encoding' => '8Bit',
    'X-Mailer' => 'Drupal',
  ];

  $mail_params = [
    'headers' => $headers,
    'form_state' => $form_state
  ];

  //drupal_mail('ereolen_support', 'ereolen_support_publizon_support', variable_get('publizon_support_email'), $language, $mail_params, $from = variable_get('site_mail'));

  // Sometimes send to Jira.
  // Create issue in Jira.
  if (!empty($jira_user)) {
    $jira_result = _ereolen_support_send_to_jira($form, $form_state, $jira_user);
    if(!empty($jira_result)) {
      drupal_set_message(variable_get('ereolen_support_submission_text'), 'status');
    }
  }
}
