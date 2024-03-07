<?php

/**
 * @file
 * Add eReolen support form.
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
 *   Support form array.
 *
 * @throws \Exception
 */
function ereolen_support_form(array $form, array &$form_state) {
  $form['#attributes'] = [
    'name' => 'ereolen_support_form',
    'class' => ['node-webform'],
    'onsubmit' => 'op.disabled = true; return true;',
  ];

  $form['#attached']['css'] = [
    drupal_get_path('module', 'ereolen_support') . '/css/styles.css',
  ];
  $form['#attached']['js'] = [
    drupal_get_path('module', 'ereolen_support') . '/js/collectDeviceData.js',
    [
      'type' => 'file',
      'data' => [
        'ereolen_support' => [
          'form_state' => $form_state,
        ],
      ],
    ],
    drupal_get_path('module', 'ereolen_support') . '/js/showHelpText.js',
    [
      'type' => 'file',
    ],
    drupal_get_path('module', 'ereolen_support') . '/js/hideOptions.js',
    [
      'type' => 'file',
    ],
    drupal_get_path('module', 'ereolen_support') . '/js/clearFields.js',
    [
      'type' => 'file',
    ],
  ];

  $form['ereolen_support_lead'] = [
    '#markup' => theme('ereolen_support_lead_text', [
      'text' => variable_get('ereolen_support_lead')['value'] ?? NULL,
    ]),
  ];

  $form['ereolen_support_problem'] = [
    '#type' => 'select',
    '#title' => t('Problem'),
    '#options' => _ereolen_support_create_select_list('ereolen_support_problem', FALSE),
    '#description' => isset(variable_get('ereolen_support_problem_description')['value']) ? variable_get('ereolen_support_problem_description')['value'] : '',
    '#required' => TRUE,
  ];

  $vacabulary = taxonomy_vocabulary_machine_name_load('ereolen_support_problem');
  $problemValues = taxonomy_get_tree($vacabulary->vid);
  $form['ereolen_support_help_text'] = [
    '#markup' => theme('ereolen_support_help_text', [
      'problem_values' => $problemValues,
    ]),
  ];

  $form['ereolen_support_name'] = [
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => isset(variable_get('ereolen_support_name_description')['value']) ? variable_get('ereolen_support_name_description')['value'] : '',
    '#required' => TRUE,
    '#element_validate' => ['_ereolen_support_check_cpr'],
  ];

  $form['ereolen_support_email'] = [
    '#type' => 'emailfield',
    '#title' => t('Email'),
    '#description' => isset(variable_get('ereolen_support_email_description')['value']) ? variable_get('ereolen_support_email_description')['value'] : '',
    '#required' => TRUE,
    '#default_value' => _ereolen_support_get_email(),
  ];

  $form['ereolen_support_date'] = [
    '#type' => 'date_popup',
    '#title' => t('Dato'),
    '#date_format' => 'd/m/Y',
    '#date_label_position' => 'none',
    '#description' => isset(variable_get('ereolen_support_date_description')['value']) ? variable_get('ereolen_support_date_description')['value'] : '',
    '#required' => TRUE,
    '#default_value' => (new \DateTime())->format('Y-m-d'),
  ];

  $libraries = module_exists('publizon') ? publizon_get_libraries_list() : [];
  $form['ereolen_support_library'] = [
    '#type' => 'select',
    '#title' => t('Library'),
    '#options' => _ereolen_support_array_as_select($libraries),
    '#description' => isset(variable_get('ereolen_support_library_description')['value']) ? variable_get('ereolen_support_library_description')['value'] : '',
    '#states' => [
      'visible' => [
        ':input[name="ereolen_support_problem"]' => [
          ['value' => 'Log ind'],
          ['value' => 'Lån og reservationer'],
          ['value' => 'Kvoter'],
        ],
      ],
      'required' => [
        ':input[name="ereolen_support_problem"]' => [
          ['value' => 'Log ind'],
          ['value' => 'Lån og reservationer'],
          ['value' => 'Kvoter'],
        ],
      ],
    ],
    '#element_validate' => [
      '_ereolen_support_states_require_validate',
    ],
    '#default_value' => _ereolen_support_get_library(),
  ];

  $form['ereolen_support_id'] = [
    '#type' => 'textfield',
    '#title' => t('Support ID'),
    '#description' => isset(variable_get('ereolen_support_id_description')['value']) ? variable_get('ereolen_support_id_description')['value'] : '',
    '#default_value' => _ereolen_support_get_support_id(),
    '#states' => [
      'visible' => [
        ':input[name="ereolen_support_problem"]' => [
          ['value' => 'Lån og reservationer'],
          ['value' => 'Fejlbeskeder'],
          ['value' => 'Kvoter'],
        ],
      ],
      'required' => [
        ':input[name="ereolen_support_problem"]' => [
          ['value' => 'Lån og reservationer'],
        ],
      ],
    ],
    '#element_validate' => [
      '_ereolen_support_states_require_validate',
      '_ereolen_support_check_cpr',
      '_ereolen_support_support_id_validate',
    ],
  ];

  $form['ereolen_support_error_message'] = [
    '#type' => 'textfield',
    '#title' => t('Error message'),
    '#description' => isset(variable_get('ereolen_support_error_message_description')['value']) ? variable_get('ereolen_support_error_message_description')['value'] : '',
    '#states' => [
      'visible' => [
        ':input[name="ereolen_support_problem"]' => [
          ['value' => 'Lån og reservationer'],
          ['value' => 'Læs og lyt'],
          ['value' => 'Download'],
          ['value' => 'Fejlbeskeder'],
          ['value' => 'Kvoter'],
        ],
      ],
      'required' => [
        ':input[name="ereolen_support_problem"]' => [
          ['value' => 'Fejlbeskeder'],
        ],
      ],
    ],
    '#element_validate' => [
      '_ereolen_support_states_require_validate',
      '_ereolen_support_check_cpr',
    ],
  ];

  $form['ereolen_support_login_method'] = [
    '#type' => 'select',
    '#title' => t('Login method'),
    '#options' => _ereolen_support_create_select_list('ereolen_support_login_method'),
    '#description' => isset(variable_get('ereolen_support_login_method_description')['value']) ? variable_get('ereolen_support_login_method_description')['value'] : '',
    '#states' => [
      'visible' => [
        ':input[name="ereolen_support_problem"]' => [
          ['value' => 'Log ind'],
          ['value' => 'Lån og reservationer'],
          ['value' => 'Fejlbeskeder'],
          ['value' => 'Kvoter'],
        ],
      ],
      'required' => [
        ':input[name="ereolen_support_problem"]' => [
          ['value' => 'Log ind'],
          ['value' => 'Lån og reservationer'],
          ['value' => 'Fejlbeskeder'],
          ['value' => 'Kvoter'],
        ],
      ],
    ],
    '#element_validate' => [
      '_ereolen_support_states_require_validate',
      '_ereolen_support_check_cpr',
    ],
  ];

  $form['ereolen_support_book_type'] = [
    '#type' => 'select',
    '#title' => t('Book type'),
    '#options' => _ereolen_support_create_select_list('ereolen_support_book_type'),
    '#description' => isset(variable_get('ereolen_support_book_type_description')['value']) ? variable_get('ereolen_support_book_type_description')['value'] : '',
    '#states' => [
      'visible' => [
        ':input[name="ereolen_support_problem"]' => [
          ['value' => 'Lån og reservationer'],
          ['value' => 'Læs og lyt'],
          ['value' => 'Download'],
          ['value' => 'Forespørgsel på titel'],
          ['value' => 'Fejlbeskeder'],
          ['value' => 'Tekniske spørgsmål'],
          ['value' => 'Kvoter'],
        ],
      ],
      'required' => [
        ':input[name="ereolen_support_problem"]' => [
          ['value' => 'Læs og lyt'],
          ['value' => 'Download'],
        ],
      ],
    ],
    '#element_validate' => [
      '_ereolen_support_states_require_validate',
    ],
  ];

  $form['ereolen_support_product'] = [
    '#type' => 'select',
    '#title' => t('Product'),
    '#options' => _ereolen_support_create_select_list('ereolen_support_product'),
    '#description' => isset(variable_get('ereolen_support_product_description')['value']) ? variable_get('ereolen_support_product_description')['value'] : '',
    '#states' => [
      'visible' => [
        ':input[name="ereolen_support_problem"]' => [
          ['value' => 'Log ind'],
          ['value' => 'Lån og reservationer'],
          ['value' => 'Læs og lyt'],
          ['value' => 'Download'],
          ['value' => 'Fejlbeskeder'],
          ['value' => 'Tekniske spørgsmål'],
          ['value' => 'Kvoter'],
        ],
      ],
      'required' => [
        ':input[name="ereolen_support_problem"]' => [
          ['value' => 'Log ind'],
          ['value' => 'Lån og reservationer'],
          ['value' => 'Læs og lyt'],
          ['value' => 'Download'],
          ['value' => 'Fejlbeskeder'],
          ['value' => 'Kvoter'],
        ],
      ],
    ],
    '#element_validate' => [
      '_ereolen_support_states_require_validate',
    ],
  ];

  $form['ereolen_support_book_title'] = [
    '#type' => 'textfield',
    '#title' => t('What book does the problem concern?'),
    '#description' => isset(variable_get('ereolen_support_book_title_description')['value']) ? variable_get('ereolen_support_description_description')['value'] : '',
    '#states' => [
      'visible' => [
        ':input[name="ereolen_support_problem"]' => [
          ['value' => 'Lån og reservationer'],
          ['value' => 'Læs og lyt'],
          ['value' => 'Download'],
          ['value' => 'Forespørgsel på titel'],
          ['value' => 'Fejlbeskeder'],
          ['value' => 'Tekniske spørgsmål'],
          ['value' => 'Kvoter'],
        ],
      ],
      'required' => [
        ':input[name="ereolen_support_problem"]' => [
          ['value' => 'Lån og reservationer'],
          ['value' => 'Læs og lyt'],
          ['value' => 'Download'],
        ],
      ],
    ],
    '#element_validate' => [
      '_ereolen_support_check_cpr',
      '_ereolen_support_states_require_validate',
    ],
  ];

  $form['ereolen_support_description'] = [
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => isset(variable_get('ereolen_support_description_description')['value']) ? variable_get('ereolen_support_description_description')['value'] : '',
    '#states' => [
      'visible' => [
        ':input[name="ereolen_support_problem"]' => [
          ['value' => 'Log ind'],
          ['value' => 'Lån og reservationer'],
          ['value' => 'Læs og lyt'],
          ['value' => 'Download'],
          ['value' => 'Forespørgsel på titel'],
          ['value' => 'Fejlbeskeder'],
          ['value' => 'Tekniske spørgsmål'],
          ['value' => 'Kvoter'],
        ],
      ],
      'required' => [
        ':input[name="ereolen_support_problem"]' => [
          ['value' => 'Log ind'],
          ['value' => 'Lån og reservationer'],
          ['value' => 'Forespørgsel på titel'],
          ['value' => 'Fejlbeskeder'],
          ['value' => 'Tekniske spørgsmål'],
          ['value' => 'Kvoter'],
        ],
      ],
    ],
    '#element_validate' => [
      '_ereolen_support_check_cpr',
      '_ereolen_support_states_require_validate',
    ],
  ];

  $form['ereolen_support_device'] = [
    '#type' => 'select',
    '#title' => t('What device did you use?'),
    '#options' => _ereolen_support_create_select_list('ereolen_support_device'),
    '#description' => isset(variable_get('ereolen_support_device_description')['value']) ? variable_get('ereolen_support_device_description')['value'] : '',
    '#states' => [
      'visible' => [
        ':input[name="ereolen_support_problem"]' => [
          ['value' => 'Download'],
          ['value' => 'Fejlbeskeder'],
          ['value' => 'Tekniske spørgsmål'],
        ],
      ],
      'required' => [
        ':input[name="ereolen_support_problem"]' => [
          ['value' => 'Download'],
          ['value' => 'Fejlbeskeder'],
        ],
      ],
    ],
  ];

  $form['ereolen_support_model'] = [
    '#type' => 'textfield',
    '#title' => t('On what device model did you experience the problem?'),
    '#description' => isset(variable_get('ereolen_support_model_description')['value']) ? variable_get('ereolen_support_device_description')['value'] : '',
    '#states' => [
      'visible' => [
        ':input[name="ereolen_support_problem"]' => [
          ['value' => 'Download'],
          ['value' => 'Fejlbeskeder'],
          ['value' => 'Tekniske spørgsmål'],
        ],
      ],
      'required' => [
        ':input[name="ereolen_support_problem"]' => [
          ['value' => 'Download'],
          ['value' => 'Fejlbeskeder'],
        ],
      ],
    ],
    '#element_validate' => [
      '_ereolen_support_check_cpr',
      '_ereolen_support_states_require_validate',
    ],
    '#maxlength' => 256,
  ];

  $form['ereolen_support_operating_system'] = [
    '#type' => 'textfield',
    '#title' => t('On what operating system did you experience the problem?'),
    '#description' => isset(variable_get('ereolen_support_operating_system_description')['value']) ? variable_get('ereolen_support_operating_system_description')['value'] : '',
    '#states' => [
      'visible' => [
        ':input[name="ereolen_support_problem"]' => [
          ['value' => 'Download'],
          ['value' => 'Fejlbeskeder'],
          ['value' => 'Tekniske spørgsmål'],
        ],
      ],
      'required' => [
        ':input[name="ereolen_support_problem"]' => [
          ['value' => 'Download'],
          ['value' => 'Fejlbeskeder'],
        ],
      ],
    ],
    '#element_validate' => [
      '_ereolen_support_check_cpr',
      '_ereolen_support_states_require_validate',
    ],
    '#maxlength' => 256,
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
    array('honeypot', 'time_restriction')
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
function ereolen_support_form_submit(array $form, array &$form_state) {
  $submission_text = variable_get('ereolen_support_submission_text');

  // Search for a user.
  $jiraUserId = _ereolen_support_search_user($form_state['values']['ereolen_support_email']);

  // Create user if none are found.
  if (empty($jiraUserId)) {
    $jiraUser = _ereolen_support_create_customer($form_state['values']['ereolen_support_email']);
    $jiraUserId = $jiraUser['content']->accountId;
  }

  // Create issue in Jira.
  $jira_result = _ereolen_support_send_to_jira($form, $form_state, $jiraUserId);
  if (!empty($jira_result)) {
    drupal_set_message($submission_text);
  }
  else {
    drupal_set_message(t('Submission failed'));
  }
}
