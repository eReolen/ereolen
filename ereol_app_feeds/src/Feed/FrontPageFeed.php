<?php

/**
 * @TODO: Missing file description doc-block?
 */

namespace Drupal\ereol_app_feeds\Feed;

use Drupal\ereol_app_feeds\Helper\ParagraphHelper;

/**
 * Class FrontPageFeed
 *
 * @package Drupal\ereol_app_feeds\Feed
 */
class FrontPageFeed extends AbstractFeed {
  const NODE_TYPE_INSPIRATION = 'inspiration';

  /**
   * Load only paragraphs on "inspiration" pages.
   *
   * @TODO: Missing return description?
   *
   * @return array
   */
  public static function getFrontPageIds() {
    $value = _ereol_app_feeds_variable_get('ereol_app_feeds_frontpage', 'frontpage_ids', []);

    return array_filter(array_values($value));
  }

  /**
   * Get front page data.
   *
   * @TODO: Missing return description?
   *
   * @return array
   */
  public function getData() {
    $frontPageIds = self::getFrontPageIds();
    $paragraphIds = $this->paragraphHelper->getParagraphIds($frontPageIds, self::NODE_TYPE_INSPIRATION, TRUE);

    // @TODO: Magic index values, why these for front page?
    $data = [
      'carousels' => $this->getCarousels($paragraphIds),
      'themes' => $this->getThemes($paragraphIds),
      'reviews' => $this->getReviews($paragraphIds),
      'editor' => $this->getEditors($paragraphIds),
      'videos' => $this->getVideos($paragraphIds),
      'audio' => $this->getAudios($paragraphIds),
    ];

    return $data;
  }

  /**
   * Get carousels.
   *
   * @TODO: Missing param and return description?
   *
   * @param array $paragraphIds
   *
   *
   * @return array
   */
  private function getCarousels(array $paragraphIds) {
    return $this->paragraphHelper->getParagraphsData(ParagraphHelper::PARAGRAPH_ALIAS_CAROUSEL, $paragraphIds);
  }

  /**
   * Get themes.
   *
   * @TODO: Missing param and return description?
   *
   * @param array $paragraphIds
   *
   *
   * @return array
   */
  private function getThemes(array $paragraphIds) {
    $themes = $this->paragraphHelper->getParagraphsData(ParagraphHelper::PARAGRAPH_ALIAS_THEME_LIST, $paragraphIds);

    // Prepend "Latest news".
    $latestNews = $this->paragraphHelper->getParagraphsData(ParagraphHelper::PARAGRAPH_ARTICLE_CAROUSEL, $paragraphIds);
    $themes = array_merge($latestNews, $themes);

    return array_values(array_filter($themes, function ($theme) {
      return isset($theme['list']) && $theme['list'];
    }));
  }

  /**
   * Get links.
   *
   * @TODO: Missing param and return description?
   * @TODO: This private function is not used?
   *
   * @param array $paragraphIds
   *
   *
   * @return array
   */
  private function getLinks(array $paragraphIds) {
    return $this->paragraphHelper->getParagraphsData(ParagraphHelper::PARAGRAPH_ALIAS_LINK, $paragraphIds);
  }

  /**
   * Get reviews.
   *
   * @TODO: Missing param and return description?
   *
   * @param array $paragraphIds
   *
   *
   * @return array
   */
  private function getReviews(array $paragraphIds) {
    return $this->paragraphHelper->getParagraphsData(ParagraphHelper::PARAGRAPH_REVIEW, $paragraphIds);
  }

  /**
   * Get editors.
   *
   * @TODO: Missing param and return description?
   *
   * @param array $paragraphIds
   *
   *
   * @return array
   */
  protected function getEditors(array $paragraphIds) {
    $data = [];

    $paragraphs = $this->paragraphHelper->getParagraphs(ParagraphHelper::PARAGRAPH_SPOTLIGHT_BOX, $paragraphIds);

    foreach ($paragraphs as $paragraph) {
      $item = $this->paragraphHelper->getEditor($paragraph);
      if ($item['list']) {
        $item['type'] = 'editor_recommends_list';
        $data[] = $item;
      }
    }

    return $data;
  }

  /**
   * Get videos.
   *
   * @TODO: Missing param and return description?
   *
   * @param array $paragraphIds
   *
   *
   * @return array
   */
  private function getVideos(array $paragraphIds) {
    // Wrap all videos in a fake list element.
    $list = [];
    $paragraphs = $this->paragraphHelper->getParagraphs(ParagraphHelper::PARAGRAPH_SPOTLIGHT_BOX, $paragraphIds);

    foreach ($paragraphs as $paragraph) {
      $item = $this->paragraphHelper->getVideoList($paragraph);
      if (!empty($item)) {
        $list[] = $item;
      }
    }
    if (count($list) > 0) {
      $list = array_merge(...$list);
    }

    return empty($list) ? [] : [
      [
        'guid' => ParagraphHelper::VALUE_NONE,
        'type' => 'video_list',
        'view' => ParagraphHelper::VIEW_DOTTED,
        'list' => $list,
      ],
    ];
  }

  /**
   * Get audio.
   *
   * @TODO: Missing param and return description?
   *
   * @param array $paragraphIds
   *
   *
   * @return array
   */
  private function getAudios(array $paragraphIds) {
    // Wrap all videos audio samples in a fake list element.
    $list = $this->paragraphHelper->getParagraphsData(ParagraphHelper::PARAGRAPH_ALIAS_AUDIO, $paragraphIds);

    return empty($list) ? [] : [
      [
        'guid' => ParagraphHelper::VALUE_NONE,
        'type' => 'audio_sample_list',
        'view' => ParagraphHelper::VIEW_DOTTED,
        'list' => $list,
      ],
    ];
  }

}
