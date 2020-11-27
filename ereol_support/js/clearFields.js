/**
 * @file clearFields.js
 *   Clear fields on form change.
 */
(function () {
  Drupal.behaviors.ereolenClearFields = {
    attach: function (context, settings) {
      'use strict';

      let problem = document.getElementById("edit-ereol-support-problem");
      problem.addEventListener('change', function(){
        clearFields();
      });
    }
  };

  /**
   * Clear fields.
   */
  function clearFields() {
    // Clear most textfields.
    let textFields = document.querySelectorAll('.form-type-textfield input');
    for (i = 0; i < textFields.length; ++i) {
      if(textFields[i].id !== 'edit-ereol-support-name' &&
        textFields[i].id !== 'edit-ereol-support-date-datepicker-popup-0' &&
        textFields[i].id !== 'edit-ereol-support-id')
      {
        textFields[i].value = "";
      }
    }

    // Clear most select fields.
    let selectFields = document.querySelectorAll('.form-type-select select');
    for (i = 0; i < selectFields.length; ++i) {
      if(selectFields[i].id !== 'edit-ereol-support-problem' &&
        selectFields[i].id !== 'edit-ereol-support-library')
      {
        selectFields[i].value = "_none";
      }
    }

    // Clear all textareas.
    let textAreas = document.querySelectorAll('.form-type-textarea textarea');
    for (i = 0; i < textAreas.length; ++i) {
      textAreas[i].value = "";
    }
  }

}());
