<?php

/**
 * @TODO: Missing file documentation.
 */

namespace Drupal\ereol_app_feeds\Helper;

use EntityFieldQuery;
use GuzzleHttp\Client;

/**
 * Paragraphs helper.
 */
class ParagraphHelper {
  // Nyeste nyheds karrusel.
  const PARAGRAPH_ARTICLE_CAROUSEL = 'article_carousel';
  // Lydprøve.
  const PARAGRAPH_AUDIO_PREVIEW = 'audio_preview';
  // Forfatterportræt.
  const PARAGRAPH_AUTHOR_PORTRAIT = 'author_portrait';
  // Forfattercitat.
  const PARAGRAPH_AUTHOR_QUOTE = 'author_quote';
  // Linkbox.
  const PARAGRAPH_LINKBOX = 'linkbox';
  // Materiale karrusel.
  const PARAGRAPH_MATERIAL_CAROUSEL = 'material_carousel';
  // Nyhedsbrevstilmelding.
  const PARAGRAPH_NEWSLETTER_SIGNUP = 'newsletter_signup';
  // Nyheds karrusel.
  const PARAGRAPH_PICKED_ARTICLE_CAROUSEL = 'picked_article_carousel';
  // Redaktøren anbefaler.
  const PARAGRAPH_RECOMMENDED_MATERIAL = 'recommended_material';
  // Litteratursiden anmeldelse.
  const PARAGRAPH_REVIEW = 'review';
  // Formidlingsboks.
  const PARAGRAPH_SPOTLIGHT_BOX = 'spotlight_box';
  // Video.
  const PARAGRAPH_VIDEO = 'video';

  // Paragraph aliases.
  const PARAGRAPH_ALIAS_AUDIO = self::PARAGRAPH_AUDIO_PREVIEW;
  const PARAGRAPH_ALIAS_CAROUSEL = self::PARAGRAPH_MATERIAL_CAROUSEL;
  const PARAGRAPH_ALIAS_EDITOR = self::PARAGRAPH_RECOMMENDED_MATERIAL;
  const PARAGRAPH_ALIAS_LINK = self::PARAGRAPH_LINKBOX;
  const PARAGRAPH_ALIAS_THEME_LIST = self::PARAGRAPH_PICKED_ARTICLE_CAROUSEL;

  // @TODO: dotted/scroll magic values?
  const VIEW_DOTTED = 'dotted';
  const VIEW_SCROLL = 'scroll';

  const VALUE_NONE = '';

  /**
   * The node helper.
   *
   * @var \Drupal\ereol_app_feeds\Helper\NodeHelper
   */
  protected $nodeHelper;

  /**
   * Constructor.
   */
  public function __construct() {
    $this->nodeHelper = new NodeHelper();
  }

  /**
   * Get a list of paragraph ids on a list of nodes.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param array $nids
   * @param null $node_type
   * @param bool $recurse
   * @return array
   */
  public function getParagraphIds(array $nids, $node_type = NULL, $recurse = TRUE) {
    $paragraphIds = [];
    $accumulator = function (\ParagraphsItemEntity $paragraph) use (&$paragraphIds) {
      $paragraphIds[] = $paragraph->item_id;
    };

    $entity_type = NodeHelper::ENTITY_TYPE_NODE;
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', $entity_type)
      ->propertyCondition('status', NODE_PUBLISHED)
      ->propertyCondition('nid', $nids ?: [0], 'IN');
    if (NULL !== $node_type) {
      $query->entityCondition('bundle', $node_type);
    }
    $result = $query->execute();

    if (isset($result[$entity_type])) {
      $nodes = node_load_multiple(array_keys($result[$entity_type]));
      foreach ($nodes as $node) {
        $paragraph_fields = $this->getParagraphFields($node);
        foreach ($paragraph_fields as $field_name => $field) {
          $paragraphs = $this->loadParagraphs($node, $field_name);
          $this->processParagraphs($paragraphs, TRUE, $accumulator);
        }
      }
    }

    return $paragraphIds;
  }

  /**
   * Process paragraphs, optionally recursively and depth first.
   *
   * @param \ParagraphsItemEntity[] $paragraphs
   *   The paragraphs.
   * @param bool $recurse
   *   Whether to recursively process paragraphs.
   * @param callable $accumulator
   *   Accumulator to accumulate results.
   */
  private function processParagraphs(array $paragraphs, $recurse, callable $accumulator) {
    foreach ($paragraphs as $paragraph) {
      $accumulator($paragraph);
      if ($recurse) {
        $paragraphFields = $this->getParagraphFields($paragraph);
        foreach ($paragraphFields as $field_name => $field) {
          $paragraphs = $this->loadParagraphs($paragraph, $field_name);
          $this->processParagraphs($paragraphs, $recurse, $accumulator);
        }
      }
    }
  }

  /**
   * Get paragraphs fields on an entity (node or paragraph).
   *
   * @TODO: Missing return description?
   *
   * @param object|\ParagraphsItemEntity $entity
   *   The entity.
   *
   * @return array
   */
  public function getParagraphFields($entity) {
    if ($entity instanceof \ParagraphsItemEntity) {
      $entity_type = NodeHelper::ENTITY_TYPE_PARAGRAPH;
      $bundle_name = $entity->bundle();
    }
    else {
      $entity_type = NodeHelper::ENTITY_TYPE_NODE;
      $bundle_name = $entity->type;
    }

    $paragraphFields = [];
    $fields = field_info_instances($entity_type, $bundle_name);
    foreach ($fields as $field_name => $info) {
      $field = field_info_field($field_name);
      if ($this->isParagraphsField($field)) {
        $paragraphFields[$field['field_name']] = $field;
      }
    }

    return $paragraphFields;
  }

  /**
   * Decide if a field is a paragraphs field.
   */
  private function isParagraphsField($field) {
    if (is_string($field)) {
      $field = field_info_field($field);
    }

    return 'paragraphs' === $field['type'];
  }

  /**
   * Load paragraphs from a paragraphs field on an entity.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param $entity
   * @param $field_name
   *
   * @return \ParagraphsItemEntity[]
   */
  public function loadParagraphs($entity, $field_name) {
    $paragraphs = [];

    if ($this->isParagraphsField($field_name) && isset($entity->{$field_name}[LANGUAGE_NONE])) {
      $values = $entity->{$field_name}[LANGUAGE_NONE];
      $ids = array_column($values, 'value');

      $paragraphs = paragraphs_item_load_multiple($ids);

      // @TODO: Why sort by id?
      NodeHelper::sortByIds($paragraphs, $ids, 'item_id');
    }

    return $paragraphs;
  }

  /**
   * Get data for a list of paragraphs.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param $type
   * @param array|NULL $paragraphIds
   * @param callable|NULL $filter
   *
   *
   * @return array|\ParagraphsItemEntity[]
   */
  public function getParagraphs($type, array $paragraphIds = NULL, callable $filter = NULL) {
    $paragraphs = [];

    $entity_type = NodeHelper::ENTITY_TYPE_PARAGRAPH;
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', $entity_type)
      ->entityCondition('bundle', $type)
      ->entityCondition('entity_id', $paragraphIds ?: [0], 'IN');
    $result = $query->execute();

    if (isset($result[$entity_type])) {
      $paragraphs = paragraphs_item_load_multiple(array_keys($result[$entity_type]));
      if (NULL !== $filter) {
        $paragraphs = array_filter($paragraphs, $filter);
      }
    }

    return $paragraphs;
  }

  /**
   * Get data for a list of paragraphs.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param $type
   * @param array $paragraphIds
   * @return array
   */
  public function getParagraphsData($type, array $paragraphIds) {
    $paragraphs = $this->getParagraphs($type, $paragraphIds);
    $data = array_values(array_map([$this, 'getParagraphData'], $paragraphs));

    // Make sure that all items are numeric arrays.
    foreach ($data as &$datum) {
      if ($this->isAssoc($datum)) {
        $datum = [$datum];
      }
    }

    // Flatten the data.
    return empty($data) ? [] : array_merge(...$data);
  }

  /**
   * Get data for a single paragraph.
   *
   * @TODO: Missing param doc-block.
   *
   * @param \ParagraphsItemEntity $paragraph
   *
   *
   * @return array|null
   *   The paragraph data. @TODO: but it might return NULL?
   */
  public function getParagraphData(\ParagraphsItemEntity $paragraph) {
    $bundle = $paragraph->bundle();

    switch ($bundle) {
      case self::PARAGRAPH_ALIAS_AUDIO:
        return $this->getAudio($paragraph);

      case self::PARAGRAPH_AUTHOR_PORTRAIT:
        // @TODO: getAuthorPortrait function not found?
        return $this->getAuthorPortrait();

      case self::PARAGRAPH_AUTHOR_QUOTE:
        // @TODO: getAuthorQuote function not found?
        return $this->getAuthorQuote();

      case self::PARAGRAPH_ALIAS_CAROUSEL:
        return $this->getCarousel($paragraph);

      case self::PARAGRAPH_ALIAS_EDITOR:
        return $this->getEditor($paragraph);

      case self::PARAGRAPH_ALIAS_LINK:
        return $this->getLink($paragraph);

      case self::PARAGRAPH_REVIEW:
        return $this->getReview($paragraph);

      case self::PARAGRAPH_SPOTLIGHT_BOX:
        return $this->getSpotlightBox($paragraph);

      case self::PARAGRAPH_ARTICLE_CAROUSEL:
        return $this->getArticleCarousel($paragraph);

      case self::PARAGRAPH_ALIAS_THEME_LIST:
        return $this->getThemeList($paragraph);

      case self::PARAGRAPH_VIDEO:
        return $this->getVideo($paragraph);
    }

    return NULL;
  }

  /**
   * Get carousel data.
   *
   * @TODO: Missing param doc-block.
   *
   * @param \ParagraphsItemEntity $paragraph
   *
   *
   * @return array
   */
  private function getCarousel(\ParagraphsItemEntity $paragraph) {
    $data = [];

    if (isset($paragraph->field_carousel[LANGUAGE_NONE])) {
      foreach ($paragraph->field_carousel[LANGUAGE_NONE] as $index => $value) {
        $data[] = [
          'guid' => $this->getGuid($paragraph, $index),
          'type' => $this->getType($paragraph),
          'title' => $this->getTitle($value['title']),
          'view' => $this->getView($paragraph),
          'query' => $value['search'],
        ];
      }
    }

    return $data;
  }

  /**
   * Get article carousel (aka "Latest news").
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param \ParagraphsItemEntity $paragraph
   * @return array
   */
  private function getArticleCarousel(\ParagraphsItemEntity $paragraph) {
    $list = [];
    // Cf. ereol_article_get_articles().
    $query = new EntityFieldQuery();
    $count = _ereol_app_feeds_variable_get('ereol_app_feeds_frontpage', 'max_news_count', 6);

    $entityType = NodeHelper::ENTITY_TYPE_NODE;
    $query->entityCondition('entity_type', 'node')
      ->entityCondition('bundle', 'article')
      ->propertyCondition('status', NODE_PUBLISHED)
      ->propertyOrderBy('created', 'DESC')
      ->range(0, $count);
    $result = $query->execute();
    if (isset($result[$entityType])) {
      $nodes = entity_load($entityType, array_keys($result[$entityType]));
      $list = array_values(array_map([$this, 'getThemeData'], $nodes));
    }

    return [
      'guid' => $this->getGuid($paragraph),
      'type' => $this->getType($paragraph),
      'view' => $this->getView($paragraph),
      'list' => $list,
    ];
  }

  /**
   * Get list of all nodes referenced from af theme paragraph.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param \ParagraphsItemEntity $paragraph
   * @return array
   */
  private function getThemeList(\ParagraphsItemEntity $paragraph) {
    $items = $this->nodeHelper->loadReferences($paragraph, 'field_picked_articles');

    $list = array_values(array_map([$this, 'getThemeData'], $items));
    // Remove items with no identifiers.
    $list = array_values(array_filter($list, function (array $item) {
      return isset($item['identifiers']);
    }));

    // @TODO: So we load more data then we need? Would it be possible to sort
    //        and do range() in the query loading nodes.
    $list = array_slice($list, 0, (int) _ereol_app_feeds_variable_get('ereol_app_feeds_frontpage', 'theme_list_max_length', 6));

    return [
      'guid' => $this->getGuid($paragraph),
      'type' => $this->getType($paragraph),
      'view' => $this->getView($paragraph),
      'list' => $list,
    ];
  }

  /**
   * Get theme data.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param $node
   * @return array
   */
  public function getThemeData($node) {
    $view = $this->nodeHelper->getFieldValue($node, 'field_image_teaser', 'value') ? 'image' : 'covers';

    $contentType = $this->nodeHelper->getFieldValue($node, 'field_article_type', 'value');
    $type = $this->nodeHelper->getThemeType($contentType);

    return [
      'guid' => $node->nid,
      'type' => $type,
      'title' => $this->getTitle($node->title),
      'view' => $view,
      'image' => $this->nodeHelper->getImage($node->field_ding_news_list_image),
      'body' => $this->nodeHelper->getBody($node),
      'identifiers' => $this->nodeHelper->getTingIdentifiers($node, 'field_ding_news_materials'),
    ];
  }

  /**
   * Get link data.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param \ParagraphsItemEntity $paragraph
   * @return array
   */
  private function getLink(\ParagraphsItemEntity $paragraph) {
    $buttonText = $this->nodeHelper->getFieldValue($paragraph, 'field_button_text', 'value');
    if (empty($buttonText)) {
      $buttonText = _ereol_app_feeds_variable_get('ereol_app_feeds_frontpage', 'link_button_text', self::VALUE_NONE);
    }

    return [
      'guid' => $this->getGuid($paragraph),
      'type' => $this->getType($paragraph),
      'title' => $this->getTitle($this->nodeHelper->getFieldValue($paragraph, 'field_link', 'title')),
      'url' => $this->getAbsoluteUrl($this->nodeHelper->getFieldValue($paragraph, 'field_link', 'url')),
      'color' => ltrim($this->nodeHelper->getFieldValue($paragraph, 'field_link_color', 'rgb'), '#'),
      'subtitle' => $this->nodeHelper->getFieldValue($paragraph, 'field_link_text', 'value'),
      'button_text' => $buttonText,
    ];
  }

  /**
   * Get absolute url.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param $url
   * @return string
   */
  private function getAbsoluteUrl($url) {
    return url($url, ['absolute' => TRUE]);
  }

  /**
   * Get review data.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param \ParagraphsItemEntity $paragraph
   * @return array
   */
  private function getReview(\ParagraphsItemEntity $paragraph) {
    return [
      'guid' => $this->getGuid($paragraph),
      'type' => 'review_list',
      'view' => $this->getView($paragraph),
      'list' => $this->getReviewList($paragraph),
    ];
  }

  /**
   * Get review list.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param \ParagraphsItemEntity $paragraph
   * @return array
   */
  private function getReviewList(\ParagraphsItemEntity $paragraph) {
    $list = [];
    if ($reviews = reol_review_get_random_reviews()) {
      foreach ($reviews as $review) {
        // @var \TingEntity $ting
        $ting = $review->ting_entity;
        $creator = $ting->getCreators() ? implode(', ', $ting->getCreators()) : self::VALUE_NONE;

        // @TODO: Why this filtering? So sometimes it's 'Litteratursiden' else a link?
        $source = preg_match('@//litteratursiden.dk/@', $review->link) ? 'Litteratursiden' : $review->link;
        $list[] = [
          'guid' => $review->rrid,
          'type' => 'review',
          'identifier' => $review->ding_entity_id,
          'title' => $review->title,
          'creator' => $creator,
          'description' => $review->description,
          'source' => $source,
          'url' => $review->link,
        ];
      }
    }

    return $list;
  }

  /**
   * @TODO: Missing function documentation?
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param \ParagraphsItemEntity $paragraph
   * @return array
   */
  public function getSpotlightBox(\ParagraphsItemEntity $paragraph) {
    $list = $this->getVideoList($paragraph);
    $videos = empty($list) ? [] : [
      [
        'guid' => ParagraphHelper::VALUE_NONE,
        'type' => 'video_list',
        'view' => ParagraphHelper::VIEW_DOTTED,
        'list' => $list,
      ],
    ];
    $links = array_values(array_map([$this, 'getLink'], $this->loadParagraphs($paragraph, 'field_spotlight_row_2')));

    $editor = self::VALUE_NONE;
    $item = $this->getEditor($paragraph);
    if ($item['list']) {
      $item['type'] = 'editor_recommends_list';
      $item['guid'] .= '-editor';
      $editor = $item;
    }

    return [
      'guid' => $this->getGuid($paragraph),
      'type' => $this->getType($paragraph),
      'view' => $this->getView($paragraph),
      'videos' => $videos,
      'reviews' => $this->getReviewList($paragraph),
      'links' => $links,
      'editor' => $editor,
    ];
  }

  /**
   * Get editor data.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param \ParagraphsItemEntity $paragraph
   * @return array
   */
  public function getEditor(\ParagraphsItemEntity $paragraph) {
    return [
      'guid' => $this->getGuid($paragraph),
      'type' => $this->getType($paragraph),
      'view' => $this->getView($paragraph),
      'list' => $this->getEditorList($paragraph),
    ];
  }

  /**
   * Get editor list.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param \ParagraphsItemEntity $paragraph
   * @return array
   */
  private function getEditorList(\ParagraphsItemEntity $paragraph) {
    $subParagraphIds = $this->nodeHelper->getFieldValue($paragraph, 'field_spotlight_row_3', 'value', TRUE);
    $subParagraphs = $this->getParagraphs(ParagraphHelper::PARAGRAPH_RECOMMENDED_MATERIAL, $subParagraphIds);
    $list = array_values(array_map(function (\ParagraphsItemEntity $subParagraph) {
      return [
        'guid' => $this->getGuid($subParagraph),
        'type' => 'editor_recommends',
        'title' => $this->getTitle($this->nodeHelper->getTextFieldValue($subParagraph, 'field_rec_title')),
        'identifier' => $this->nodeHelper->getTingIdentifierFromUrl($this->nodeHelper->getTextFieldValue($subParagraph, 'field_rec_material')),
      ];
    }, $subParagraphs));

    return $list;
  }

  /**
   * Get editor data.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param \ParagraphsItemEntity $paragraph
   * @return array
   */
  protected function getVideo(\ParagraphsItemEntity $paragraph) {
    // Wrap all videos in a fake list element.
    return [
      'guid' => $this->getGuid($paragraph),
      'type' => $this->getType($paragraph),
      'view' => $this->getView($paragraph),
      'list' => $this->getVideoList($paragraph),
    ];
  }

  /**
   * Get video data.
   *
   * @TODO: Missing param doc-block.
   *
   * @param \ParagraphsItemEntity $paragraph
   *
   * @return array
   *   The data.
   */
  public function getVideoList(\ParagraphsItemEntity $paragraph) {
    $subParagraphIds = $this->nodeHelper->getFieldValue($paragraph, 'field_spotlight_primary', 'value', TRUE);
    $subParagraphs = $this->getParagraphs(ParagraphHelper::PARAGRAPH_VIDEO, $subParagraphIds);
    $list = array_values(array_map(function (\ParagraphsItemEntity $subParagraph) {
      $video = $this->nodeHelper->loadReferences($subParagraph, 'field_video_node', FALSE);
      $url = $this->nodeHelper->getFieldValue($video, 'field_video', 'uri');
      return [
        'guid' => $this->getGuid($subParagraph),
        'type' => $this->getType($subParagraph),
        'title' => $this->getTitle($video->title),
        'image' => self::VALUE_NONE,
        'source' => $this->getVideoSource($url),
        'url' => $this->nodeHelper->getUrl($url),
      ];
    }, $subParagraphs));

    return $list;
  }

  /**
   * Get video source.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param $url
   * @return string
   */
  private function getVideoSource($url) {
    if (preg_match('/(?P<source>youtube)/', $url, $matches)) {
      return $matches['source'];
    }

    return self::VALUE_NONE;
  }

  /**
   * Get audio data.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param \ParagraphsItemEntity $paragraph
   * @return array|null
   */
  private function getAudio(\ParagraphsItemEntity $paragraph) {
    $url = $this->nodeHelper->getTextFieldValue($paragraph, 'field_preview_material');
    $identifier = $this->nodeHelper->getTingIdentifierFromUrl($url);
    if (NULL !== $identifier) {
      $ting = $this->nodeHelper->loadTingObject($identifier);
      if ($ting) {
        $isbn = $ting->getIsbn();
        $isbn = reset($isbn);

        list($audioUrl, $metadata) = $this->getAudioMetadata($isbn);

        return [
          'guid' => $this->getGuid($paragraph),
          'type' => $this->getType($paragraph),
          'identifier' => $identifier,
          'title' => $this->getTitle($ting->getTitle()),
          'url' => $audioUrl,
          'metadata' => $metadata,
        ];
      }
    }

    return NULL;
  }

  /**
   * Get audio metadata.
   *
   * Warning: this performs 2 http requests!
   *
   * @param string $isbn
   *   The isbn.
   *
   * @return array
   *   [audio url, metadata] or [null, null]
   */
  private function getAudioMetadata($isbn) {
    $audioMetadata = &drupal_static(__METHOD__);

    /**
     * @TODO: Use the Publizon client
     *
     * $client = \PublizonProductClient::getClient();
     * $product = $client->getProduct($isbn);
     *
     * If the PublizonProduct calls don't parse the need data it may need to be
     * extended in it's parse function. Currently it only parse the data that
     * have been needed.
     *
     * If the PublizonProduct can not be used the data fetched here should be
     * cached with cache_set/cache_get as the data is not likely to change.
     */

    if (!isset($audioMetadata[$isbn])) {
      try {
        $metadata = [];

        $metadataUrl = 'https://audio.api.streaming.pubhub.dk/v1/samples/' . $isbn;
        $audioUrl = 'https://audio.api.streaming.pubhub.dk/Sample.ashx?isbn=' . $isbn;

        $client = new Client();
        $response = $client->get($metadataUrl);
        $data = json_decode((string) $response->getBody(), TRUE);

        $metadata['length'] = $data['duration'];

        $response = $client->head($audioUrl);
        $header = $response->getHeader('content-range');
        $contentRange = reset($header);
        if (preg_match('@bytes (?P<range_start>[0-9]+)-(?P<range_end>[0-9]+)/(?P<size>[0-9]+)@',
                       $contentRange, $matches)) {
          $metadata['size'] = (int) $matches['size'];
        }

        $header = $response->getHeader('content-type');
        $metadata['format'] = reset($header);

        $audioMetadata[$isbn] = [$audioUrl, $metadata];
      }
      catch (\Exception $exception) {
        // @TODO: why catch all?
      }
    }

    return isset($audioMetadata[$isbn]) ? $audioMetadata[$isbn] : [NULL, NULL];
  }

  /**
   * Get guid (generally unique id) for a paragraph.
   *
   * The guid is NOT guaranteed bo be globally unique.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param \ParagraphsItemEntity $paragraph
   * @param null $delta
   * @return string|null
   */
  public function getGuid(\ParagraphsItemEntity $paragraph, $delta = NULL) {
    $guid = $paragraph->identifier();
    if (NULL !== $delta) {
      $guid .= '-' . $delta;
    }

    return $guid;
  }

  /**
   * Get data type for a paragraph.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param \ParagraphsItemEntity $paragraph
   * @return string|null
   */
  private function getType(\ParagraphsItemEntity $paragraph) {
    $bundle = $paragraph->bundle();

    switch ($bundle) {
      case self::PARAGRAPH_ALIAS_AUDIO:
        return 'audio_sample';

      case self::PARAGRAPH_AUTHOR_PORTRAIT:
        return 'author_portrait';

      case self::PARAGRAPH_AUTHOR_QUOTE:
        return 'author_quote';

      case self::PARAGRAPH_ALIAS_CAROUSEL:
        return 'carousel';

      case self::PARAGRAPH_ALIAS_EDITOR:
        return 'editor';

      case self::PARAGRAPH_ALIAS_LINK:
        return 'link';

      case self::PARAGRAPH_REVIEW:
        return 'review';

      case self::PARAGRAPH_SPOTLIGHT_BOX:
        return 'spotlight_box';

      case self::PARAGRAPH_ARTICLE_CAROUSEL:
      case self::PARAGRAPH_ALIAS_THEME_LIST:
        return 'theme_list';

      case self::PARAGRAPH_VIDEO:
        return 'video';
    }

    return NULL;
  }

  /**
   * Get view for a paragraph.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param \ParagraphsItemEntity $paragraph
   * @return string
   */
  public function getView(\ParagraphsItemEntity $paragraph) {
    $bundle = $paragraph->bundle();

    switch ($bundle) {
      case self::PARAGRAPH_ALIAS_CAROUSEL:
        return self::VIEW_SCROLL;

      case self::PARAGRAPH_ALIAS_THEME_LIST:
      case self::PARAGRAPH_AUTHOR_PORTRAIT:
      case self::PARAGRAPH_AUTHOR_QUOTE:
      case self::PARAGRAPH_ALIAS_EDITOR:
      case self::PARAGRAPH_ALIAS_LINK:
      case self::PARAGRAPH_REVIEW:
      case self::PARAGRAPH_SPOTLIGHT_BOX:
      case self::PARAGRAPH_ARTICLE_CAROUSEL:
      case self::PARAGRAPH_VIDEO:
        return self::VIEW_DOTTED;
    }

    return self::VIEW_SCROLL;
  }

  /**
   * Get real paragraph type from an alias.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @TODO: is the const and string not the same?
   *
   * @param $alias
   * @return string
   */
  public function getParagraphType($alias) {
    switch ($alias) {
      case 'audio':
        return self::PARAGRAPH_ALIAS_AUDIO;

      case 'author_portrait':
        return self::PARAGRAPH_AUTHOR_PORTRAIT;

      case 'author_quote':
        return self::PARAGRAPH_AUTHOR_QUOTE;

      case 'carousel':
        return self::PARAGRAPH_ALIAS_CAROUSEL;

      case 'editor':
        return self::PARAGRAPH_ALIAS_EDITOR;

      case 'link':
        return self::PARAGRAPH_ALIAS_LINK;

      case 'picked_article_carousel':
        return self::PARAGRAPH_ALIAS_THEME_LIST;

      case 'review':
        return self::PARAGRAPH_REVIEW;

      case 'spotlight_box':
        return self::PARAGRAPH_SPOTLIGHT_BOX;

      case 'theme':
        return self::PARAGRAPH_ARTICLE_CAROUSEL;

      case 'theme_list':
        return self::PARAGRAPH_ALIAS_THEME_LIST;

      case 'video':
        return self::PARAGRAPH_VIDEO;
    }

    return $alias;
  }

  /**
   * Decode entities in title.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param $title
   * @return string
   */
  private function getTitle($title) {
    return html_entity_decode($title);
  }

  /**
   * Check if an array is associative.
   *
   * @param array $arr
   *   The array to check.
   *
   * @return bool
   *   True iff the array is associative.
   *
   * @see https://stackoverflow.com/a/173479
   */
  private function isAssoc(array $arr = NULL) {
    if (NULL === $arr || [] === $arr) {
      return FALSE;
    }
    ksort($arr);
    return array_keys($arr) !== range(0, count($arr) - 1);
  }

}
