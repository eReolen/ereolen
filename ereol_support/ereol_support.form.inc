<?php

/**
 * @file
 * Add eReolen support form.
 */

include_once 'ereol_support.helper.inc';
include_once 'ereol_support.cpr-checker.inc';

/**
 * Create frontend form.
 *
 * @param array $form
 *   The form to create.
 * @param array $form_state
 *   The state of the form.
 *
 * @return array
 *   The form.
 *
 * @throws \Exception
 */
function ereol_support_form(array $form, array &$form_state) {
  $form['#attributes'] = [
    'name' => 'ereol_support_form',
    'class' => ['node-webform'],
  ];

  $form['#attached']['css'] = [
    drupal_get_path('module', 'ereol_support') . '/css/styles.css',
  ];
  $form['#attached']['js'] = [
    [
      'data' => drupal_get_path('module', 'ereol_support') . '/js/collectDeviceData.js',
      'type' => 'file',
    ],
    [
      'data' => drupal_get_path('module', 'ereol_support') . '/js/showHelpText.js',
      'type' => 'file',
    ],
    [
      'data' => drupal_get_path('module', 'ereol_support') . '/js/hideOptions.js',
      'type' => 'file',
    ],
    [
      'data' => drupal_get_path('module', 'ereol_support') . '/js/clearFields.js',
      'type' => 'file',
    ],
  ];

  $form['ereolen_support_lead'] = [
    '#markup' => theme('ereolen_support_lead_text', [
      'text' =>  variable_get('ereolen_support_lead')['value'] ?? NULL,
    ]),
  ];

  $form['ereol_support_problem'] = [
    '#type' => 'select',
    '#title' => t('Problem'),
    '#options' => _ereol_support_create_select_list('ereol_support_problem', FALSE),
    '#description' => variable_get('ereol_support_problem_description')['value'] ?? NULL,
    '#required' => TRUE,
  ];

  $vacabulary = taxonomy_vocabulary_machine_name_load('ereol_support_problem');
  $problemValues = taxonomy_get_tree($vacabulary->vid);
  $form['ereol_support_help_text'] = [
    '#markup' => theme('ereol_support_help_text', [
      'problem_values' => $problemValues,
    ]),
  ];

  $form['ereol_support_name'] = [
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => variable_get('ereol_support_name_description')['value'] ?? NULL,
    '#required' => TRUE,
    '#element_validate' => ['_ereol_support_check_cpr'],
  ];

  $form['ereol_support_email'] = [
    '#type' => 'emailfield',
    '#title' => t('Email'),
    '#description' => variable_get('ereol_support_email_description')['value'] ?? NULL,
    '#required' => TRUE,
    '#default_value' => _ereol_support_get_email(),
  ];

  $form['ereol_support_date'] = [
    '#type' => 'date_popup',
    '#title' => t('Dato'),
    '#date_format' => 'd/m/Y',
    '#date_label_position' => 'none',
    '#description' => variable_get('ereol_support_date_description')['value'] ?? NULL,
    '#required' => TRUE,
    '#default_value' => (new \DateTime())->format('Y-m-d'),
  ];

  $libraries = module_exists('publizon') ? publizon_get_libraries_list() : [];
  $form['ereol_support_library'] = [
    '#type' => 'select',
    '#title' => t('Library'),
    '#options' => _ereol_support_array_as_select($libraries),
    '#description' => variable_get('ereol_support_library_description')['value'] ?? NULL,
    '#states' => [
      'visible' => [
        ':input[name="ereol_support_problem"]' => [
          ['value' => 'Log ind'],
          ['value' => 'Lån og reservationer'],
          ['value' => 'Kvoter'],
        ],
      ],
      'required' => [
        ':input[name="ereol_support_problem"]' => [
          ['value' => 'Log ind'],
          ['value' => 'Lån og reservationer'],
          ['value' => 'Kvoter'],
        ],
      ],
    ],
    '#element_validate' => [
      '_ereol_support_states_require_validate',
    ],
    '#default_value' => _ereol_support_get_library(),
  ];

  $form['ereol_support_id'] = [
    '#type' => 'textfield',
    '#title' => t('Support ID'),
    '#description' => variable_get('ereol_support_id_description')['value'] ?? NULL,
    '#default_value' => _ereol_support_get_support_id(),
    '#states' => [
      'visible' => [
        ':input[name="ereol_support_problem"]' => [
          ['value' => 'Lån og reservationer'],
          ['value' => 'Fejlbeskeder'],
          ['value' => 'Kvoter'],
        ],
      ],
      'required' => [
        ':input[name="ereol_support_problem"]' => [
          ['value' => 'Lån og reservationer'],
        ],
      ],
    ],
    '#element_validate' => [
      '_ereol_support_states_require_validate',
      '_ereol_support_check_cpr',
      '_ereol_support_support_id_validate',
    ],
  ];

  $form['ereol_support_error_message'] = [
    '#type' => 'textfield',
    '#title' => t('Error message'),
    '#description' => variable_get('ereol_support_error_message_description')['value'] ?? NULL,
    '#states' => [
      'visible' => [
        ':input[name="ereol_support_problem"]' => [
          ['value' => 'Lån og reservationer'],
          ['value' => 'Læs og lyt'],
          ['value' => 'Download'],
          ['value' => 'Fejlbeskeder'],
          ['value' => 'Kvoter'],
        ],
      ],
      'required' => [
        ':input[name="ereol_support_problem"]' => [
          ['value' => 'Fejlbeskeder'],
        ],
      ],
    ],
    '#element_validate' => [
      '_ereol_support_states_require_validate',
      '_ereol_support_check_cpr',
    ],
  ];

  $form['ereol_support_login_method'] = [
    '#type' => 'select',
    '#title' => t('Login method'),
    '#options' => _ereol_support_create_select_list('ereol_support_login_method'),
    '#description' => variable_get('ereol_support_login_method_description')['value'] ?? NULL,
    '#states' => [
      'visible' => [
        ':input[name="ereol_support_problem"]' => [
          ['value' => 'Log ind'],
          ['value' => 'Lån og reservationer'],
          ['value' => 'Fejlbeskeder'],
          ['value' => 'Kvoter'],
        ],
      ],
      'required' => [
        ':input[name="ereol_support_problem"]' => [
          ['value' => 'Log ind'],
          ['value' => 'Lån og reservationer'],
          ['value' => 'Fejlbeskeder'],
          ['value' => 'Kvoter'],
        ],
      ],
    ],
    '#element_validate' => [
      '_ereol_support_states_require_validate',
      '_ereol_support_check_cpr',
    ],
  ];

  $form['ereol_support_book_type'] = [
    '#type' => 'select',
    '#title' => t('Book type'),
    '#options' => _ereol_support_create_select_list('ereol_support_book_type'),
    '#description' => variable_get('ereol_support_book_type_description')['value'] ?? NULL,
    '#states' => [
      'visible' => [
        ':input[name="ereol_support_problem"]' => [
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
        ':input[name="ereol_support_problem"]' => [
          ['value' => 'Læs og lyt'],
          ['value' => 'Download'],
        ],
      ],
    ],
    '#element_validate' => [
      '_ereol_support_states_require_validate',
    ],
  ];

  $form['ereol_support_product'] = [
    '#type' => 'select',
    '#title' => t('Product'),
    '#options' => _ereol_support_create_select_list('ereol_support_product'),
    '#description' => variable_get('ereol_support_product_description')['value'] ?? NULL,
    '#states' => [
      'visible' => [
        ':input[name="ereol_support_problem"]' => [
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
        ':input[name="ereol_support_problem"]' => [
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
      '_ereol_support_states_require_validate',
    ],
  ];

  $form['ereol_support_description'] = [
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => variable_get('ereol_support_description_description')['value'] ?? NULL,
    '#states' => [
      'visible' => [
        ':input[name="ereol_support_problem"]' => [
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
        ':input[name="ereol_support_problem"]' => [
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
      '_ereol_support_check_cpr',
      '_ereol_support_states_require_validate',
    ],
  ];

  $form['ereol_support_book_title'] = [
    '#type' => 'textfield',
    '#title' => t('What book does the problem concern?'),
    '#description' => variable_get('ereol_support_book_title_description')['value'] ?? NULL,
    '#states' => [
      'visible' => [
        ':input[name="ereol_support_problem"]' => [
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
        ':input[name="ereol_support_problem"]' => [
          ['value' => 'Lån og reservationer'],
          ['value' => 'Læs og lyt'],
          ['value' => 'Download'],
        ],
      ],
    ],
    '#element_validate' => [
      '_ereol_support_check_cpr',
      '_ereol_support_states_require_validate',
    ],
  ];

  $form['ereol_support_device'] = [
    '#type' => 'select',
    '#title' => t('What device did you use?'),
    '#options' => _ereol_support_create_select_list('ereol_support_device'),
    '#description' => variable_get('ereol_support_device_description')['value'] ?? NULL,
    '#states' => [
      'visible' => [
        ':input[name="ereol_support_problem"]' => [
          ['value' => 'Download'],
          ['value' => 'Fejlbeskeder'],
          ['value' => 'Tekniske spørgsmål'],
        ],
      ],
      'required' => [
        ':input[name="ereol_support_problem"]' => [
          ['value' => 'Download'],
          ['value' => 'Fejlbeskeder'],
        ],
      ],
    ],
  ];

  $form['ereol_support_model'] = [
    '#type' => 'textfield',
    '#title' => t('On what device model did you experience the problem?'),
    '#description' => variable_get('ereol_support_model_description')['value'] ?? NULL,
    '#states' => [
      'visible' => [
        ':input[name="ereol_support_problem"]' => [
          ['value' => 'Download'],
          ['value' => 'Fejlbeskeder'],
          ['value' => 'Tekniske spørgsmål'],
        ],
      ],
      'required' => [
        ':input[name="ereol_support_problem"]' => [
          ['value' => 'Download'],
          ['value' => 'Fejlbeskeder'],
        ],
      ],
    ],
    '#element_validate' => [
      '_ereol_support_check_cpr',
      '_ereol_support_states_require_validate',
    ],
    '#maxlength' => 256,
  ];

  $form['ereol_support_operating_system'] = [
    '#type' => 'textfield',
    '#title' => t('On what operating system did you experience the problem?'),
    '#description' => variable_get('ereol_support_operating_system_description')['value'] ?? NULL,
    '#states' => [
      'visible' => [
        ':input[name="ereol_support_problem"]' => [
          ['value' => 'Download'],
          ['value' => 'Fejlbeskeder'],
          ['value' => 'Tekniske spørgsmål'],
        ],
      ],
      'required' => [
        ':input[name="ereol_support_problem"]' => [
          ['value' => 'Download'],
          ['value' => 'Fejlbeskeder'],
        ],
      ],
    ],
    '#element_validate' => [
      '_ereol_support_check_cpr',
      '_ereol_support_states_require_validate',
    ],
    '#maxlength' => 256,
  ];

  $form['submit_button'] = [
    '#type' => 'submit',
    '#value' => t('Send'),
  ];

  honeypot_add_form_protection($form, $form_state, array('honeypot', 'time_restriction'));

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
function ereol_support_form_submit(array $form, array &$form_state) {
  global $user;
  $submission_text = variable_get('ereol_support_submission_text');
  $error_message = variable_get('ereol_support_submission_error_message');
  // Switch between jira and email to publizon.
  if ($form_state['input']['ereol_support_problem'] == 'Læs og lyt') {
    try {
      $language = $user->uid ? user_preferred_language($user) : language_default();
      $headers = [
        'MIME-Version' => '1.0',
        'Content-Type' => 'text/html; charset=UTF-8; format=flowed; delsp=yes',
        'Content-Transfer-Encoding' => '8Bit',
        'X-Mailer' => 'Drupal',
      ];

      $mail_params = [
        'headers' => $headers,
        'form_state' => $form_state,
      ];

      drupal_mail('ereol_support', 'ereol_support_publizon_support', variable_get('publizon_support_email'), $language, $mail_params, $from = variable_get('site_mail'));
      drupal_set_message($submission_text, 'status');
    }
    catch (Throwable $t) {
      drupal_set_message($error_message, 'error');
    }
  }
  else {
    try {
      // Check for and create user in Jira
      $jira_user = _ereolen_support_search_jira_user($user, $form_state, $form_state['values']['ereolen_support_email']);
      if (!$jira_user) {
        $jira_user = _ereolen_support_create_jira_user($user, $form_state, $form_state['values']['ereolen_support_email']);
      }

      // Create issue in Jira.
      if (!empty($jira_user)) {
        $jira_result = _ereol_support_send_to_jira($form, $form_state, $jira_user);
        if (!empty($jira_result)) {
          drupal_set_message($submission_text, 'status');
          return;
        }
      }
      drupal_set_message($error_message, 'error');

    }
    catch (Throwable $t) {
      drupal_set_message($error_message, 'error');
    }
  }
}
