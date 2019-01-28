<?php

namespace Drupal\ereol_app_feeds\Controller;

use Drupal\ereol_app_feeds\Feed\CategoriesFeed;
use Drupal\ereol_app_feeds\Feed\FrontPageFeed;
use Drupal\ereol_app_feeds\Feed\ThemesFeed;

/**
 * Default controller.
 */
class DefaultController {

  /**
   * Render front page data.
   */
  public function frontpage() {
    $feed = new FrontPageFeed();
    $data = $feed->getData();

    drupal_json_output($data);
    drupal_exit();
  }

  /**
   * Render themes data.
   */
  public function themes() {
    $feed = new ThemesFeed();
    $data = $feed->getData();

    drupal_json_output($data);
    drupal_exit();
  }

  /**
   * Render categories data.
   */
  public function categories() {
    $feed = new CategoriesFeed();
    $data = $feed->getData();

    drupal_json_output($data);
    drupal_exit();
  }

}
