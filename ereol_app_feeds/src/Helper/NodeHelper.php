<?php

/**
 * @TODO: Missing @file doc-block.
 *
 * @TODO: It looks like a lot of the function in the class is doing the work
 *        that's normally handled with the entity_metadata_wrapper?
 * @See https://www.drupal.org/docs/7/api/entity-api/entity-metadata-wrappers
 */

namespace Drupal\ereol_app_feeds\Helper;

use EntityFieldQuery;

/**
 * Node helper.
 */
class NodeHelper {
  const ENTITY_TYPE_NODE = 'node';
  const ENTITY_TYPE_PARAGRAPH = 'paragraphs_item';

  /**
   * Get value of a field.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param $entity
   * @param $field_name
   * @param null $sub_field_name
   * @param bool $multiple
   * @return array|mixed|null
   */
  public function getFieldValue($entity, $field_name, $sub_field_name = NULL, $multiple = FALSE) {
    if (!isset($entity->{$field_name}[LANGUAGE_NONE])) {
      return NULL;
    }

    $values = $entity->{$field_name}[LANGUAGE_NONE];

    if (NULL !== $sub_field_name) {
      $values = array_column($values, $sub_field_name);
    }

    return $multiple ? $values : reset($values);
  }

  /**
   * Get text value of a field.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param $entity
   * @param $field_name
   * @param null $sub_field_name
   * @param bool $multiple
   * @return array|mixed
   */
  public function getTextFieldValue($entity, $field_name, $sub_field_name = NULL, $multiple = FALSE) {
    $values = $this->getFieldValue($entity, $field_name, $sub_field_name, TRUE);
    $values = array_map([$this, 'getTextValue'], $values);

    return $multiple ? $values : reset($values);
  }

  /**
   * Get list of paragraphs on a node.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param $entity
   * @return array|\ParagraphsItemEntity[]
   */
  public function getParagraphs($entity) {
    $paragraphs = [];

    $helper = new ParagraphHelper();

    $fields = $helper->getParagraphFields($entity);
    foreach ($fields as $field_name => $field) {
      $paragraphs += $helper->loadParagraphs($entity, $field_name);
    }

    return $paragraphs;
  }

  /**
   * Get text value.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param $value
   * @return null
   */
  private function getTextValue($value) {
    return isset($value['safe_value']) ? $value['safe_value'] : NULL;
  }

  /**
   * Get body from a node.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param $node
   * @return array|mixed
   */
  public function getBody($node) {
    return $this->getTextFieldValue($node, 'body', NULL, FALSE);
  }

  /**
   * Get image url.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param $value
   * @param bool $multiple
   * @return array|mixed|null
   */
  public function getImage($value, $multiple = FALSE) {
    if (!isset($value[LANGUAGE_NONE])) {
      return NULL;
    }
    $values = $value[LANGUAGE_NONE];
    $uris = array_column($values, 'uri');
    $urls = array_map([$this, 'getUrl'], $uris);

    return $multiple ? $urls : reset($urls);
  }

  /**
   * Get an absolute url from a "public:/" url.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param $url
   * @return bool|string
   */
  public function getUrl($url) {
    return file_create_url($url);
  }

  /**
   * Get ting identifiers.
   *
   * @TODO: Missing param doc-block.
   *
   * @param $entity
   * @param $field_name
   *
   * @return string[]
   *   A list of identifiers.
   */
  public function getTingIdentifiers($entity, $field_name) {
    if (!isset($entity->{$field_name}[LANGUAGE_NONE])) {
      return [];
    }

    $relations = $this->getFieldValue($entity, $field_name, 'value', TRUE);

    $ids = array_map(function ($relation) {
      if ((isset($relation->relation_type) && 'ting_reference' === $relation->relation_type)
        && (isset($relation->endpoints[LANGUAGE_NONE][1]['entity_type']) && 'ting_object' === $relation->endpoints[LANGUAGE_NONE][1]['entity_type'])
      ) {
        return $relation->endpoints[LANGUAGE_NONE][1]['entity_id'];
      }

      return NULL;
    }, $relations);

    $result = db_query('SELECT o.ding_entity_id FROM {ting_object} o WHERE o.tid in (:ids)', [':ids' => $ids]);
    $identifiers = $result->fetchCol();

    // Filter out any non "basis" identifiers.
    // @TODO: Why filter out non basic?
    $identifiers = array_filter($identifiers, function ($identifier) {
      return preg_match('/-basis:/', $identifier);
    });

    return array_values($identifiers);
  }

  /**
   * Load a ting object by identifier.
   *
   * @param string $identifier
   *   Ting identifier.
   *
   * @return \TingEntity|null
   *   The ting object if any.
   */
  public function loadTingObject($identifier) {
    if (!empty($identifier)) {
      $entity_type = 'ting_object';
      $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', $entity_type)
        ->propertyCondition('ding_entity_id', $identifier);
      $result = $query->execute();

      //@TODO: Maybe look into ding_entity_load()/ding_entity_load_multiple().
      if (isset($result[$entity_type])) {
        $entities = entity_load($entity_type, array_keys($result[$entity_type]));
        return reset($entities);
      }
    }

    return NULL;
  }

  /**
   * Get a ting identifier from a url.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param $url
   * @return string|null
   */
  public function getTingIdentifierFromUrl($url) {
    return preg_match('@/object/(?P<identifier>.+)$@', $url, $matches) ? urldecode($matches['identifier']) : NULL;
  }

  /**
   * Load nodes from references.
   *
   * @param object|\ParagraphsItemEntity $entity
   *   The entity.
   * @param string $field_name
   *   The field name.
   * @param bool $multiple
   *   It set, load multiple references. Otherwise, load just one.
   *
   * @return mixed
   *   A single node or a list of nodes if any.
   */
  public function loadReferences($entity, $field_name, $multiple = TRUE) {
    if (!isset($entity->{$field_name}[LANGUAGE_NONE])) {
      return NULL;
    }
    $values = $entity->{$field_name}[LANGUAGE_NONE];
    $nids = array_column($values, 'target_id');
    $nodes = node_load_multiple($nids);

    return $multiple ? $nodes : reset($nodes);
  }

  /**
   * Load a single node of a specific type by nid.
   *
   * @param string $node_type
   *   The node type.
   * @param int $nid
   *   The node id.
   *
   * @return bool|mixed|null
   *   The node if any.
   */
  public function loadNode($node_type, $nid) {
    $entity_type = self::ENTITY_TYPE_NODE;
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', $entity_type)
      ->entityCondition('bundle', $node_type)
      ->propertyCondition('status', NODE_PUBLISHED)
      ->entityCondition('entity_id', $nid);
    $result = $query->execute();

    // @TODO: Why not use entity_load?
    return isset($result[$entity_type][$nid]) ? node_load($nid) : NULL;
  }

  /**
   * Load nodes ordered by specified order of node ids.
   *
   * @TODO: Missing some param doc-block.
   *
   * @param $nids
   *   The node ids.
   * @param int $status
   *
   * @return array
   *   An array of node objects indexed by nid.
   */
  public function loadNodes(array $nids, $status = NODE_PUBLISHED) {
    $entity_type = self::ENTITY_TYPE_NODE;
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', $entity_type)
      ->propertyCondition('status', $status)
      ->entityCondition('entity_id', $nids + [0]);
    $result = $query->execute();

    $nodes = isset($result[$entity_type]) ? node_load_multiple(array_keys($result[$entity_type])) : [];

    // @TODO: Why is the sort based on nid needed?
    self::sortByIds($nodes, $nids);

    return $nodes;
  }

  /**
   * @TODO: Missing doc-block.
   *
   * @param array $items
   * @param array $ids
   * @param string $id_key
   */
  public static function sortByIds(array &$items, array $ids, $id_key = 'nid') {
    // Order by index in $nids.
    uasort($items, function ($a, $b) use ($ids, $id_key) {
      $a = array_search($a->{$id_key}, $ids);
      $b = array_search($b->{$id_key}, $ids);

      return $a - $b;
    });
  }

  /**
   * Map from article content type to theme type.
   *
   * @var array
   */
  private static $themeTypes = [
    'article' => 'theme',
    'author_portrait' => 'author_theme',
    'news' => 'theme',
  ];

  /**
   * Get theme type.
   *
   * @TODO: Missing param and return doc-block.
   *
   * @param $contentType
   * @return mixed|string
   */
  public function getThemeType($contentType) {
    return isset(self::$themeTypes[$contentType]) ? self::$themeTypes[$contentType] : 'theme';
  }

}
