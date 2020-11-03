/**
 * @file clearFields.js
 *   Clear fields on form change.
 */
(function () {
  Drupal.behaviors.ereolenClearFields = {
    attach: function (context, settings) {
      'use strict';

      let problem = document.getElementById("edit-ereolen-support-problem");
      clearFields();

      problem.addEventListener('change', function(){
        clearFields();
      });
    }
  };

  /**
   * Clear fields.
   */
  function clearFields() {
    let textFields = document.querySelectorAll('.form-type-textfield input');
    let selectFields = document.querySelectorAll('.form-type-select select');
    let textAreas = document.querySelectorAll('.form-type-textarea textarea');
    // Clear most textfields.
    for (i = 0; i < textFields.length; ++i) {
      if(textFields[i].id !== 'edit-ereolen-support-name'
        && textFields[i].id !== 'edit-ereolen-support-date-datepicker-popup-0') {
        textFields[i].value = "";
      }
    }
    // Clear most select fields.
    for (i = 0; i < selectFields.length; ++i) {
      if(selectFields[i].id !== 'edit-ereolen-support-problem'
        && selectFields[i].id !== 'edit-ereolen-support-library') {
        selectFields[i].value = "_none";
      }
    }
    // Clear all textareas.
    for (i = 0; i < textAreas.length; ++i) {
      textAreas[i].value = "";
    }
  }

}());
