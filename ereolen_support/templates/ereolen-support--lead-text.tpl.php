<?php
/**
 * @file
 * Template for showing form lead text.
 */
?>
<div class="form-item">
  <?php $text = isset(variable_get('ereolen_support_lead')['value']) ? variable_get('ereolen_support_lead')['value'] : '' ?>
  <?php if (!empty($text)) : ?>
    <div class="ereolen-support--lead-text text">
      <?php print $text; ?>
    </div>
  <?php endif; ?>
</div>

