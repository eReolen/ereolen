<?php
/**
 * @file
 * Template for email sent to publizon.
 *
 * Available variables:
 *   $time: Now - Format long.
 *   $user: The current user.
 *   $form_input: The support form submission values.
 */
?>
<p>
  <?php print t('Submitted on');?> <?php print $time; ?><br>
  <?php print t('Submitted by user');?>: <?php print $user->name; ?><br>
  <?php print t('Provided values');?>:<br>
</p>
<p>
  <?php print t('Library');?>: <?php print $form_input['ereolen_support_library']; ?><br>
  <?php print t('Support id');?>: <?php print $form_input['ereolen_support_id']; ?><br>
  <?php print t('Name');?>: <?php print $form_input['ereolen_support_name']; ?><br>
  <?php print t('Email');?>: <?php print $form_input['ereolen_support_email']; ?><br>
  <?php print t('What is the problem?');?>: <?php print $form_input['ereolen_support_problem']; ?><br>
  <?php print t('Title');?>: <?php print $form_input['ereolen_support_book_title']; ?><br>
  <?php print t('Description');?>: <?php print $form_input['ereolen_support_description']; ?><br>
  <?php print t('Time');?>: <?php print $form_input['ereolen_support_date']['date']; ?><br>
  <?php print t('Device');?>: <?php print $form_input['ereolen_support_device']; ?><br>
  <?php print t('Model no.');?>: <?php print $form_input['ereolen_support_model']; ?><br>
  <?php print t('Operating system');?>: <?php print $form_input['ereolen_support_operating_system']; ?>
</p>
