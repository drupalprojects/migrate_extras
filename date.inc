<?php

/**
 * @file
 * Support for migration into Date fields.
 */

class MigrateDateFieldHandler extends MigrateFieldHandler {
  public function __construct() {
    $this->registerTypes(array('date', 'datestamp', 'datetime'));
  }

  /*
   * Arguments for a date field migration.
   *
   * @param timezone
   *   Timezone (such as UTC, America/New_York, etc.) to apply.
   * @param to
   *   The To date of a date range
   * @param rrule
   *   Rule string for a repeating date field.
   */
  static function arguments($timezone = 'UTC', $to = NULL, $rrule = NULL) {
    return get_defined_vars();
  }

  /**
   * Convert incoming data into the proper field arrays for Date fields.
   *
   * @param $entity
   *  The destination entity which will hold the field arrays.
   * @param array $field_info
   *  Metadata for the date field being populated.
   * @param array $instance
   *  Metadata for this instance of the date field being populated.
   * @param array $values
   *  Array of date values to be fielded.
   */
  public function prepare($entity, array $instance, array $values) {
    if (isset($values['arguments'])) {
      $arguments = $values['arguments'];
      unset($values['arguments']);
    }
    else {
      $arguments = array();
    }
    if (isset($arguments['timezone'])) {
      $timezone = $arguments['timezone'];
    }
    else {
      $timezone = 'UTC';
    }
    if (isset($arguments['to'])) {
      $value2 = $arguments['to'];
    }
    else {
      $value2 = NULL;
    }
    if (isset($arguments['rrule'])) {
      $rrule = $arguments['rrule'];
    }
    else {
      $rrule = NULL;
    }

    // Convert the timezone name to an offset
    $date = date_now($timezone);
    $timezone = date_offset_get($date);

    // Setup the standard Field API array for saving.
    $delta = 0;
    foreach ($values as $value) {
      // Work from a timestamp
      $value = MigrationBase::timestamp($value);
      if ($value2) {
        $value2 = MigrationBase::timestamp($value2);
      }

      // What does the destination field expect?
      switch ($instance['type']) {
        case 'datestamp':
          // Already done
          break;
        case 'datetime':
          // YYYY-MM-DD HH:MM:SS
          $value = format_date($value, 'custom', 'Y-m-d H:i', $timezone);
          if ($value2) {
            $value2 = format_date($value2, 'custom', 'Y-m-d H:i', $timezone);
          }
          break;
        case 'date':
          // ISO date: YYYY-MM-DDTHH:MM:SS
          $value = format_date($value, 'custom', 'Y-m-d\TH:i', $timezone);
          if ($value2) {
            $value2 = format_date($value2, 'custom', 'Y-m-d\TH:i', $timezone);
          }
          break;
        default:
          break;
      }

      // Handle repeats, coming in as RRULEs. Many field instances may be created.
      if ($instance['widget']['type'] == 'date_text_repeat' && $rrule) {
        include_once(drupal_get_path('module', 'date') . '/date_repeat.inc');
        include_once(drupal_get_path('module', 'date_api') . '/date_api_ical.inc');
        $item = array('value' => $value, 'value2' => $value2, 'timezone' => $timezone);
        // Can be de-uglified when http://drupal.org/node/1159404 is committed.
        $return = date_repeat_build_dates(NULL, date_ical_parse_rrule(NULL, $rrule),
          $instance, $item);
      }
      else {
        $return[$delta]['value'] = $value;
        $return[$delta]['value2'] = $value2;
      }
      $delta++;
    }
    if (!isset($return)) {
      $return = NULL;
    }
    return $return;
  }
}
