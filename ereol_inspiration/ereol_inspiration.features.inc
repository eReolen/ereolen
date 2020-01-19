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
 * Implements hook_image_default_styles().
 */
function ereol_inspiration_image_default_styles() {
  $styles = array();

  // Exported image style: app_feed_image.
  $styles['app_feed_image'] = array(
    'label' => 'App feed image',
    'effects' => array(
      2 => array(
        'name' => 'manualcrop_crop_and_scale',
        'data' => array(
          'width' => 500,
          'height' => 500,
          'upscale' => 0,
          'respectminimum' => 1,
          'onlyscaleifcrop' => 0,
          'style_name' => 'app_feed_image',
        ),
        'weight' => 0,
      ),
    ),
  );

  return $styles;
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
