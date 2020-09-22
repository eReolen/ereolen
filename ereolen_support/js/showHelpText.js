/**
 * @file Show help text.
 *   Show hide help texts depending on problem selected.
 */
(function () {
  Drupal.behaviors.ereolenShowHelp = {
    attach: function (context, settings) {
      'use strict';

      let problem = document.getElementById("edit-ereolen-support-problem");
      let texts = document.getElementsByClassName('js-ereolen-support--help-text');
      hideAll(texts);

      problem.addEventListener('change', function(){
        hideAll(texts);
        let problemValue = problem.options[problem.selectedIndex].value;
        for (let i = 0; i < texts.length; i++) {
          let dataSelector = texts.item(i).getAttribute('data-problem');
          if (dataSelector === problemValue) {
            texts.item(i).style.display = 'block';
          }
        }
      });
    }
  };

  /**
   * Hide all texts
   *
   * @param texts
   *   The texts to hide.
   */
  function hideAll(texts) {
    for (let i = 0; i < texts.length; i++) {
      texts.item(i).style.display = 'none';
    }
  }

}());
