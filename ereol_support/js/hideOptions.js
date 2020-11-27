/**
 * @file hideOptions.js
 *   Hide certain select options in certain situations.
 */
(function () {
  Drupal.behaviors.hideOptions = {
    attach: function (context, settings) {
      'use strict';

      let problem = document.getElementById("edit-ereol-support-problem");
      hideOptions(problem);

      problem.addEventListener('change', function(){
        hideOptions(problem);
      });
    }
  };

  /**
   * Hide options
   */
  function hideOptions(problem) {
    let productOptions = document.getElementById("edit-ereol-support-product").getElementsByTagName("option");
    if (problem.value === 'Download') {
      for (let i = 0; i < productOptions.length; i++) {
        if (productOptions[i].value === "ereolen.dk" ||
          productOptions[i].value === "ereolenglobal.dk" ||
          productOptions[i].value === "ereolengo.dk"
        ){
          productOptions[i].disabled = true;
        }
      }
    } else {
      for (let i = 0; i < productOptions.length; i++) {
        productOptions[i].removeAttribute('disabled');
      }
    }
  }

}());
