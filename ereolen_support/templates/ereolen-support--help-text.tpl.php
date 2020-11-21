<?php

/**
 * @file
 * Template for showing a help message to the user.
 */
?>
<div class="form-item">
  <?php foreach ($problem_values as $problem) : ?>
    <?php $text = variable_get('ereolen_support_problem_help_' . $problem->tid)['value'] ?? NULL ?>
    <?php if (!empty($text)) : ?>
      <div style="display: none;" class="ereolen-support--help-text js-ereolen-support--help-text" data-problem="<?php print $problem->name; ?>">
        <?php print $text; ?>
      </div>
    <?php endif; ?>
  <?php endforeach; ?>
</div>
