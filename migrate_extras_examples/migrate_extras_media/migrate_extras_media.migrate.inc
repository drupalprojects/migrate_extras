<?php

/*
 * Implementation of hook_migrate_api().
 */
function migrate_extras_media_migrate_api() {
  $api = array(
    'api' => 2,
  );
  return $api;
}

abstract class MigrateExampleMediaMigration extends XMLMigration {
  public function __construct() {
    parent::__construct();
  }
}

abstract class MigrateExampleMediaCommonMigration extends MigrateExampleMediaMigration {
  public function __construct($item_xpath) {
    parent::__construct();

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'href' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
        )
      ),
      MigrateDestinationMedia::getKeySchema()
    );

    $fields = array(
      'href' => 'URL of file to import',
      'description' => 'Description of file',
    );

    $xml_folder = drupal_get_path('module', 'migrate_extras_media');
    $items_url = $xml_folder . '/migrate_extras_media.xml';
    $item_ID_xpath = '@href';
    $items_class = new MigrateItemsXML($items_url, $item_xpath, $item_ID_xpath);
    $this->source = new MigrateSourceMultiItems($items_class, $fields);
    $this->destination = new MigrateDestinationMedia('image',
      array('copy_file' => TRUE));

    // Basic fields
    $this->addFieldMapping('uid')
         ->defaultValue(1);
    $this->addFieldMapping('uri', 'href')
         ->xpath('@href');
    $this->addFieldMapping('filename', 'href')
         ->xpath('@href');

    $this->addUnmigratedDestinations(array('contents', 'filemime', 'status', 'timestamp',
                                     'path'));
  }
}

/**
 * Migration classes to test import of media entities.
 */
class MigrateExampleImageMigration extends MigrateExampleMediaCommonMigration {
  public function __construct() {
    parent::__construct('/source_data/item/image');
    $this->description = t('Example migration into image media entities');

    $this->addFieldMapping('field_image_description', 'description')
         ->xpath('description');
  }
}

/**
 * Migration class to test import of media entities.
 */
class MigrateExampleMediaNodeMigration extends XMLMigration {
  public function __construct() {
    parent::__construct();
    $this->description = t('Example migration into media entities');
    $this->dependencies = array('MigrateExampleImage');

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'id' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
        )
      ),
      MigrateDestinationMedia::getKeySchema()
    );

    $fields = array(
      'id' => '',
      'title' => '',
      'body' => '',
    );

    // Our test data is in an XML file
    $xml_folder = drupal_get_path('module', 'migrate_extras_media');
    $items_url = $xml_folder . '/migrate_extras_media.xml';
    $item_xpath = '/source_data/item';
    $item_ID_xpath = 'id';
    $items_class = new MigrateItemsXML($items_url, $item_xpath, $item_ID_xpath);
    $this->source = new MigrateSourceMultiItems($items_class, $fields);
    $this->destination = new MigrateDestinationNode('migrate_example_media');


    // Basic fields
    $this->addFieldMapping('title', 'title')
         ->xpath('title');
    $this->addFieldMapping('uid')
         ->defaultValue(1);
    $this->addFieldMapping('body', 'body')
         ->xpath('body');

    // Media fields
    $this->addFieldMapping('field_image', 'image')
         ->xpath('image@href')
         ->sourceMigration('MigrateExampleImage');

    // No unmapped source fields

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('is_new', 'status', 'promote',
      'revision', 'language', 'sticky', 'created', 'changed', 'revision_uid'));
  }
}
