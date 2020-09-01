<?php
/**
 * @file
 * CPR checker for form element validation.
 */

/**
 * Check a form element input for CPR numbers.
 *
 * @param $element
 *   The form element to check.
 * @param $form_state
 *   The state of the form.
 */
function _ereolen_support_check_cpr($element, &$form_state) {
  $value = $element['#value'];
  if (numberChk($value)) {
    form_error($element, t('%name contains a number that could well be a CPR number. Please change the text.', array(
      '%name' => $element['#title'],
    )));
  }
}

/**
 * Check if string contains valid CPR.
 *
 * @param $string
 *   A string input by user.
 *
 * @return bool
 *   True if string contains a number that could be considered cpr.
 */
function numberChk($string) {
  if(empty($string)) {
    return FALSE;
  }

  // Remove spaces and dashes to prepare for a simpler regex.
  $stringConcatenated = str_replace(['-',' '], '', $string);

  // Search the concatenated string for 10 digits in a row.
  $numberFound = preg_match('/(^|\D)\d{10}($|\D)/', $stringConcatenated, $result);

  if($numberFound) {
    $number = $result[0];

    // Remove prefix from number.
    if (!empty($result[1])) {
      $number = substr($number, 1);
    }

    // Remove suffix from number.
    if (!empty($result[2])) {
      $number = substr($number, 0, -1);
    }

    // Prepare number for modulo 11 check.
    $arr = stringSplit($number);

    if (mod11Chk($arr, $number) && dateChk($number)) {
      return TRUE;
    }
    else {
      return FALSE;
    }
  }
}

/**
 * Create an array from a string.
 *
 * @param $numString
 *   A string to create an array from.
 *
 * @return array
 *   The created array of characters.
 */
function stringSplit($numString) {
  $array = [];
  for ($i = 0; $i < strlen($numString); $i++) {
    $array[$i] = substr($numString, $i, 1);
  }

  return $array;
}

/**
 * Check against modulo 11.
 *
 * @param $array
 *   An array of characters.
 *
 * @param $number
 *   A string that could be a CPR number.
 *
 * @return bool
 *   Whether the characters in union resemble a CPR number.
 *
 */
function mod11Chk($array, $number) {
  // Due to lacking capacity of valid modulo 11 numbers some years, there are
  // valid CPR numbers that don't pass modulo 11 check. In those cases we just
  // return true.
  // https://cpr.dk/cpr-systemet/personnumre-uden-kontrolciffer-modulus-11-kontrol/
  $noModuloCheckNumbers = [
    '010160',
    '010164',
    '010165',
    '010166',
    '010169',
    '010170',
    '010174',
    '010180',
    '010182',
    '010184',
    '010185',
    '010186',
    '010187',
    '010188',
    '010189',
    '010190',
    '010191',
    '010192',
  ];

  if (in_array(substr($number, 0, 6), $noModuloCheckNumbers)) {
    return TRUE;
  }
  // Check each digit against it's weight.
  // Modulo 11 weights for CPR: 4, 3, 2, 7, 6, 5, 4, 3, 2, 1
  $value = 0;
  foreach($array as $key => $v) {
    switch($key) {
      case 0:
        $value += $v * 4;
        break;
      case 1:
        $value += $v * 3;
        break;
      case 2:
        $value += $v * 2;
        break;
      case 3:
        $value += $v * 7;
        break;
      case 4:
        $value += $v * 6;
        break;
      case 5:
        $value += $v * 5;
        break;
      case 6:
        $value += $v * 4;
        break;
      case 7:
        $value += $v * 3;
        break;
      case 8:
        $value += $v * 2;
        break;
      case 9:
        $value += $v * 1;
        break;
    }
  }

  // Check the sum against modulo 11, if remainder is 0 the number resembles CPR.
  if ($value % 11 == 0) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

/**
 * Check against valid date.
 *
 * @param $number
 *   A string that could be a CPR number.
 *
 * @return bool
 */
function dateChk($number) {
  $day = substr($number, 0, 2);
  $month = substr($number, 2, 2);
  $year = substr($number, 4, 2);

  if ($year < 21)
    $year = "20" . $year;
  else
    $year = "19" . $year;

  return checkdate($month, $day, $year);
}
