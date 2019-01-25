<?php

/**
 * @TODO: Missing file description doc-block?
 */

namespace Drupal\ereol_app_feeds\Feed;

use Drupal\ereol_app_feeds\Helper\ParagraphHelper;

/**
 * Paragraphs feed.
 */
class ParagraphsFeed extends AbstractFeed {

  /**
   * Get feed data.
   *
   * @TODO: Missing param and return doc-blocks.
   *
   * @param $nids
   * @param $type
   *
   *
   * @return array
   */
  public function getData($nids, $type) {
    $helper = new ParagraphHelper();
    $type = $helper->getParagraphType($type);
    $ids = $helper->getParagraphIds($nids);

    $data = $helper->getParagraphsData($type, $ids);

    // HACK!
    // @TODO: What is the "hack" and why is the "hack" needed?
    if (ParagraphHelper::PARAGRAPH_ALIAS_THEME_LIST === $type) {
      $lists = array_column(array_filter($data, function (array $item) {
        return isset($item['list']);
      }), 'list');

      $data = array_merge(...$lists);

      // Remove items with no identifiers.
      $data = array_filter($data, function (array $item) {
        return isset($item['identifiers']);
      });
    }

    return $data;
  }

}
