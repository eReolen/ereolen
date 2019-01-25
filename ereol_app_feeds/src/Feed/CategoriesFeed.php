<?php

/**
 * @TODO: Missing file description doc-block?
 */

namespace Drupal\ereol_app_feeds\Feed;

use Drupal\ereol_app_feeds\Helper\NodeHelper;
use Drupal\ereol_app_feeds\Helper\ParagraphHelper;

/**
 * Categories feed.
 */
class CategoriesFeed extends AbstractFeed {

  /**
   * Get feed data.
   *
   * @TODO: Missing return doc-block.
   *
   * @return array
   */
  public function getData() {
    $data = [];

    $nodes = $this->getNodes();

    foreach ($nodes as $node) {
      $paragraphs = $this->nodeHelper->getParagraphs($node);
      $content = array_values(array_map(function ($paragraph) {
        $data = $this->paragraphHelper->getParagraphData($paragraph);

        switch ($paragraph->bundle()) {
          case ParagraphHelper::PARAGRAPH_MATERIAL_CAROUSEL:
            $data = [
              'guid' => $this->paragraphHelper->getGuid($paragraph),
              'type' => $this->paragraphHelper->getParagraphType($paragraph->bundle()),
              'view' => $this->paragraphHelper->getView($paragraph),
              'list' => $data,
            ];
            break;
        }

        return $data;
      }, $paragraphs));

      $data[] = [
        'title' => $node->title,
        'content' => $content,
      ];
    }

    return $data;
  }

  /**
   * @TODO: Missing documentation?
   *
   * @return array
   */
  private function getNodes() {
    $group_name = 'ereol_app_feeds_category';
    $field_name = 'page_ids';
    $pages = _ereol_app_feeds_variable_get($group_name, $field_name, []);
    $included = array_filter($pages, function ($page) {
      return isset($page['included']) && 1 === $page['included'];
    });

    return $this->nodeHelper->loadNodes(array_keys($included));
  }

}
