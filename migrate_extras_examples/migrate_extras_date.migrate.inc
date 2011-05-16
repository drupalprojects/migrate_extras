<?php
/**
 * @file
 * Examples and test fodder for migration into date fields.
 */

/**
 * Migration class to test import of various date fields.
 */
class MigrateExampleDateMigration extends XMLMigration {
  public function __construct() {
    parent::__construct();
    $this->description = t('Example migration into date fields');

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'id' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'description' => 'Date ID',
        )
      ),
      MigrateDestinationNode::getKeySchema()
    );

    // Source fields available in the XML file.
    $fields = array(
      'id' => t('Source id'),
      'title' => t('Title'),
      'body' => t('Description'),
      'date' => t('A simple date'),
      'date_range' => t('Start value for a date range'),
      'date_range_to' => t('End value for a date range'),
      'datestamp' => t('Simple datestamp'),
      'datestamp_range' => t('Start value for a datestamp range'),
      'datestamp_range_to' => t('End value for a datestamp range'),
      'datetime' => t('Simple datetime'),
      'datetime_range' => t('Start value for a datetime range'),
      'datetime_range_to' => t('End value for a datetime range'),
      'date_repeat' => t('Sample of a repeating date field'),
      'rrule' => t('Repeat rule to be used for date_repeat'),
    );

    // Our test data is in an XML file
    $xml_folder = drupal_get_path('module', 'migrate_extras_date');
    $items_url = $xml_folder . '/migrate_extras_date.xml';
    $item_xpath = '/source_data/item';
    $item_ID_xpath = 'id';
    $items_class = new MigrateItemsXML($items_url, $item_xpath, $item_ID_xpath);
    $this->source = new MigrateSourceMultiItems($items_class, $fields);
    $this->destination = new MigrateDestinationNode('migrate_example_date');

    // Basic fields
    $this->addFieldMapping('title', 'title')
         ->xpath('title');
    $this->addFieldMapping('uid')
         ->defaultValue(1);
    $this->addFieldMapping('body', 'body')
         ->xpath('body');

    // Date field mappings

    // Simple fields, we just need the xpath
    $this->addFieldMapping('field_date', 'date')
         ->xpath('date');

    // For date ranges, we use a "to" argument to tell it what source field holds
    // the end date
    $this->addFieldMapping('field_date_range', 'date_range')
         ->xpath('date_range/from')
         ->arguments(array('to' => array('source_field' => 'date_range_to')));
    // We add a NULL mapping which holds the xpath. With a SQL source, this would
    // not be necessary - just by having the To value in the query it would be
    // available to the argument above.
    $this->addFieldMapping(NULL, 'date_range_to')
         ->xpath('date_range/to');

    // For RRULEs on repeating date fields, we do much the same as we did for the To.
    $arguments = MigrateDateFieldHandler::arguments('UTC', NULL, array('source_field' => 'rrule'));
    $this->addFieldMapping('field_date_repeat', 'date_repeat')
         ->xpath('date_repeat/date')
         ->arguments($arguments);
    $this->addFieldMapping(NULL, 'rrule')
         ->xpath('date_repeat/rule');

    $this->addFieldMapping('field_datestamp', 'datestamp')
         ->xpath('datestamp');
    $this->addFieldMapping('field_datestamp_range', 'datestamp_range')
         ->xpath('datestamp_range/from')
         ->arguments(array('to' => array('source_field' => 'datestamp_range_to')));
    $this->addFieldMapping(NULL, 'datestamp_range_to')
         ->xpath('datestamp_range/to');
    $this->addFieldMapping('field_datetime', 'datetime')
         ->xpath('datetime');
    $this->addFieldMapping('field_datetime_range', 'datetime_range')
         ->xpath('datetime_range/from')
         ->arguments(array('to' => array('source_field' => 'datetime_range_to')));
    $this->addFieldMapping(NULL, 'datetime_range_to')
         ->xpath('datetime_range/to');

    // No unmapped source fields

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('is_new', 'status', 'promote',
      'revision', 'language', 'sticky', 'created', 'changed', 'revision_uid'));
  }
}
