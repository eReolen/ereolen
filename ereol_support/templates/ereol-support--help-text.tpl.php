<?php

/**
 * @file
 * Template for showing a help message to the user.
 */
?>
<div class="form-item">
  <?php foreach ($problem_values as $problem) : ?>
    <?php $text = variable_get('ereol_support_problem_help_' . $problem->tid)['value'] ?? NULL ?>
    <?php if (!empty($text)) : ?>
      <div style="display: none;" class="ereol-support--help-text js-ereol-support--help-text" data-problem="<?php print $problem->name; ?>">
        <?php print $text; ?>
      </div>
    <?php endif; ?>
  <?php endforeach; ?>
</div>
