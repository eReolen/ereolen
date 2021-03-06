<?php

namespace Drupal\ereol_app_feeds\Feed;

use Drupal\ereol_app_feeds\Helper\NodeHelper;
use Drupal\ereol_app_feeds\Helper\ParagraphHelper;

/**
 * Abstract feed.
 */
abstract class AbstractFeed {

  /**
   * The node helper.
   *
   * @var \Drupal\ereol_app_feeds\Helper\NodeHelper
   */
  protected $nodeHelper;

  /**
   * The paragraph helper.
   *
   * @var \Drupal\ereol_app_feeds\Helper\ParagraphHelper*/
  protected $paragraphHelper;

  /**
   * Constructor.
   */
  public function __construct() {
    $this->nodeHelper = new NodeHelper();
    $this->paragraphHelper = new ParagraphHelper();
  }

  /**
   * Get feed data.
   *
   * @return array
   *   The feed data.
   */
  abstract public function getData();

}
