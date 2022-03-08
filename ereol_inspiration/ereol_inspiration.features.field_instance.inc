<?php
/**
 * @file
 * ereol_inspiration.features.field_instance.inc
 */

/**
 * Implements hook_field_default_field_instances().
 */
function ereol_inspiration_field_default_field_instances() {
  $field_instances = array();

  // Exported field_instance: 'node-inspiration-field_app_feed_color'.
  $field_instances['node-inspiration-field_app_feed_color'] = array(
    'bundle' => 'inspiration',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 2,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_app_feed_color',
    'label' => 'App feed color',
    'required' => 1,
    'settings' => array(
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 0,
      'module' => 'color_field',
      'settings' => array(),
      'type' => 'color_field_plain_text',
      'weight' => 2,
    ),
  );

  // Exported field_instance: 'node-inspiration-field_app_feed_image'.
  $field_instances['node-inspiration-field_app_feed_image'] = array(
    'bundle' => 'inspiration',
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 3,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_app_feed_image',
    'label' => 'App feed image',
    'required' => 1,
    'settings' => array(
      'alt_field' => 0,
      'default_image' => 0,
      'file_directory' => '',
      'file_extensions' => 'png',
      'max_filesize' => '',
      'max_resolution' => '',
      'min_resolution' => '500x500',
      'title_field' => 0,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'media',
      'settings' => array(
        'allowed_schemes' => array(
          'oembed' => 0,
          'public' => 'public',
          'vimeo' => 0,
          'youtube' => 0,
        ),
        'allowed_types' => array(
          'audio' => 0,
          'document' => 0,
          'image' => 'image',
          'video' => 0,
        ),
        'browser_plugins' => array(
          'ding_content' => 0,
          'media_default--media_browser_1' => 0,
          'media_default--media_browser_my_files' => 0,
          'media_internet' => 0,
          'upload' => 0,
        ),
        'manualcrop_crop_info' => 1,
        'manualcrop_default_crop_area' => 1,
        'manualcrop_enable' => 1,
        'manualcrop_inline_crop' => 0,
        'manualcrop_instant_crop' => FALSE,
        'manualcrop_instant_preview' => 1,
        'manualcrop_keyboard' => 1,
        'manualcrop_maximize_default_crop_area' => 0,
        'manualcrop_require_cropping' => array(
          'app_feed_image' => 'app_feed_image',
        ),
        'manualcrop_styles_list' => array(
          'app_feed_image' => 'app_feed_image',
        ),
        'manualcrop_styles_mode' => 'include',
        'manualcrop_thumblist' => 0,
      ),
      'type' => 'media_generic',
      'weight' => 3,
    ),
  );

  // Exported field_instance: 'node-inspiration-field_category_query'.
  $field_instances['node-inspiration-field_category_query'] = array(
    'bundle' => 'inspiration',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => 'Søgestreng for hovedkategorien',
    'display' => array(
      'default' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 1,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 0,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_category_query',
    'label' => 'Category query',
    'required' => 1,
    'settings' => array(
      'text_processing' => 0,
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 1,
      'module' => 'text',
      'settings' => array(
        'rows' => 5,
      ),
      'type' => 'text_textarea',
      'weight' => 1,
    ),
  );

  // Exported field_instance: 'node-inspiration-field_inspiration_paragraphs'.
  $field_instances['node-inspiration-field_inspiration_paragraphs'] = array(
    'bundle' => 'inspiration',
    'default_value' => NULL,
    'deleted' => 0,
    'description' => '',
    'display' => array(
      'default' => array(
        'label' => 'hidden',
        'module' => 'paragraphs',
        'settings' => array(
          'view_mode' => 'full',
        ),
        'type' => 'paragraphs_view',
        'weight' => 0,
      ),
      'teaser' => array(
        'label' => 'above',
        'settings' => array(),
        'type' => 'hidden',
        'weight' => 1,
      ),
    ),
    'entity_type' => 'node',
    'field_name' => 'field_inspiration_paragraphs',
    'label' => 'Elementer',
    'required' => 0,
    'settings' => array(
      'add_mode' => 'select',
      'allowed_bundles' => array(
        'article_carousel' => 'article_carousel',
        'audio_preview' => -1,
        'author_portrait' => -1,
        'author_quote' => -1,
        'blue_titles_info' => 'blue_titles_info',
        'linkbox' => -1,
        'material_carousel' => 'material_carousel',
        'newsletter_signup' => 'newsletter_signup',
        'picked_article_carousel' => 'picked_article_carousel',
        'recommended_material' => -1,
        'newsletter_signup' => 'newsletter_signup',
        'review' => -1,
        'spotlight_box' => 'spotlight_box',
        'video' => -1,
        'video_bundle' => 'video_bundle',
      ),
      'bundle_weights' => array(
        'article_carousel' => 2,
        'audio_preview' => 9,
        'author_portrait' => 10,
        'author_quote' => 11,
        'blue_titles_info' => 38,
        'linkbox' => 3,
        'material_carousel' => 4,
        'newsletter_signup' => 14,
        'picked_article_carousel' => -24,
        'recommended_material' => 15,
        'review' => 5,
        'spotlight_box' => 6,
        'video' => 18,
        'video_bundle' => 32,
      ),
      'default_edit_mode' => 'open',
      'title' => 'Element',
      'title_multiple' => 'Elementer',
      'user_register_form' => FALSE,
    ),
    'widget' => array(
      'active' => 0,
      'module' => 'paragraphs',
      'settings' => array(),
      'type' => 'paragraphs_embed',
      'weight' => 5,
    ),
  );

  // Translatables
  // Included for use with string extractors like potx.
  t('App feed color');
  t('App feed image');
  t('Category query');
  t('Elementer');
  t('Query for the main category');

  return $field_instances;
}
