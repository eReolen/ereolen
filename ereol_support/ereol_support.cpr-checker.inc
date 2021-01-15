<?php

/**
 * @file
 * CPR checker for form element validation.
 */

/**
 * Check a form element input for CPR numbers.
 *
 * @param array $element
 *   The form element to check.
 * @param array $form_state
 *   The state of the form.
 */
function _ereol_support_check_cpr(array $element, array &$form_state) {
  $value = $element['#value'];
  require_once(__DIR__.'/classes/CprValidator.php');
  $cprValidator = new CprValidator();
  if ($cprValidator->containsCpr($value)) {
    form_error($element, t('%name contains a number that could well be a CPR number. Please change the text.', array(
      '%name' => $element['#title'],
    )));
  }
}
