<?php
/**
 * @file
 * ereol_inspiration.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function ereol_inspiration_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_node_info().
 */
function ereol_inspiration_node_info() {
  $items = array(
    'inspiration' => array(
      'name' => t('Inspiration'),
      'base' => 'node_content',
      'description' => t('A page that provides inspiration for readers.'),
      'has_title' => '1',
      'title_label' => t('Titel'),
      'help' => '',
    ),
  );
  drupal_alter('node_info', $items);
  return $items;
}

/**
 * Implements hook_paragraphs_info().
 */
function ereol_inspiration_paragraphs_info() {
  $items = array(
    'blue_titles_info' => array(
      'name' => 'blue titles info',
      'bundle' => 'blue_titles_info',
    ),
    'category_list' => array(
      'name' => 'Category list',
      'bundle' => 'category_list',
      'locked' => '1',
    ),
  );
  return $items;
}
