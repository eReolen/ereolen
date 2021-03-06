<?php

namespace Drupal\ereol_app_feeds\Feed;

use Drupal\ereol_app_feeds\Helper\ParagraphHelper;

/**
 * Class FrontPageFeed.
 *
 * @package Drupal\ereol_app_feeds\Feed
 */
class FrontPageFeed extends AbstractFeed {
  const NODE_TYPE_INSPIRATION = 'inspiration';

  /**
   * Get front page ids from app feed settings.
   *
   * @return array
   *   The front page ids.
   */
  public static function getFrontPageIds() {
    $value = _ereol_app_feeds_variable_get('ereol_app_feeds_frontpage', 'frontpage_ids', []);

    return array_filter(array_values($value));
  }

  /**
   * Get front page feed data.
   *
   * @see https://docs.google.com/document/d/1lJ3VPAJf7DAbBWAQclRHfcltzZefUG3iGCec-z97KlA/edit?ts=5c4ef9d5#heading=h.12un1qdppa6x
   * for details on the feed structure.
   *
   * @return array
   *   The feed data.
   */
  public function getData() {
    $frontPageIds = self::getFrontPageIds();
    $paragraphIds = $this->paragraphHelper->getParagraphIds($frontPageIds, self::NODE_TYPE_INSPIRATION, TRUE);

    $data = [
      'carousels' => $this->getCarousels($paragraphIds),
      'themes' => $this->getThemes($paragraphIds),
      'reviews' => $this->getReviews(),
      'editor' => $this->getEditors($paragraphIds),
      'videos' => $this->getVideos($paragraphIds),
      'audio' => $this->getAudios($paragraphIds),
    ];

    return $data;
  }

  /**
   * Get carousels.
   *
   * @param array $paragraphIds
   *   The paragraph ids.
   *
   * @see https://docs.google.com/document/d/1lJ3VPAJf7DAbBWAQclRHfcltzZefUG3iGCec-z97KlA/edit?ts=5c4ef9d5#bookmark=id.51b3v5z38ank
   *
   * @return array
   *   The carousels data.
   */
  private function getCarousels(array $paragraphIds) {
    return $this->paragraphHelper->getParagraphsData(ParagraphHelper::PARAGRAPH_ALIAS_CAROUSEL, $paragraphIds);
  }

  /**
   * Get themes.
   *
   * @param array $paragraphIds
   *   The paragraph ids.
   *
   * @see https://docs.google.com/document/d/1lJ3VPAJf7DAbBWAQclRHfcltzZefUG3iGCec-z97KlA/edit?ts=5c4ef9d5#bookmark=id.vpgrki5b8gg
   *
   * @return array
   *   The themes data.
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
   * Get reviews.
   *
   * @see https://docs.google.com/document/d/1lJ3VPAJf7DAbBWAQclRHfcltzZefUG3iGCec-z97KlA/edit?ts=5c4ef9d5#bookmark=id.qh5qjlx68dde
   *
   * @return array
   *   The reviews data.
   */
  private function getReviews() {
    return $this->paragraphHelper->getReviewList(5);
  }

  /**
   * Get editors.
   *
   * @param array $paragraphIds
   *   The paragraph ids.
   *
   * @see https://docs.google.com/document/d/1lJ3VPAJf7DAbBWAQclRHfcltzZefUG3iGCec-z97KlA/edit?ts=5c4ef9d5#bookmark=id.isl1hf5mbnf
   *
   * @return array
   *   The editors data.
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
   * @param array $paragraphIds
   *   The paragraph ids.
   *
   * @see https://docs.google.com/document/d/1lJ3VPAJf7DAbBWAQclRHfcltzZefUG3iGCec-z97KlA/edit?ts=5c4ef9d5#bookmark=id.aw3cqhhwfwa0
   *
   * @return array
   *   The videos data.
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
   * @param array $paragraphIds
   *   The paragraph ids.
   *
   * @see https://docs.google.com/document/d/1lJ3VPAJf7DAbBWAQclRHfcltzZefUG3iGCec-z97KlA/edit?ts=5c4ef9d5#bookmark=id.awje1hlhr91
   *
   * @return array
   *   The audio data.
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
