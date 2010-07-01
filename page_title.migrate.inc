<?php
// $Id$

/**
 * @file
 * Support for titles into the Page Title module.
 */

class MigratePageTitleNodeHandler extends MigrateDestinationHandler {
  public function __construct() {
    $this->registerTypes(array('node'));
  }

  /**
   * Make the destination field visible.
   */
  public function fields() {
    return array(
      'page_title' => t('Page Title: Title for &lt;title&gt; tag'),
    );
  }
}
