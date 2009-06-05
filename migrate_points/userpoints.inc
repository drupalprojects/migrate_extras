<?php
// $Id$

/**
 * @file
 * Integrates userpoints module with the migrate module
 * 
 */


/**
 * [Full Doxygen style documentaiton: see http://drupal.org/node/1354]
 * 
 * [Do something to something else ] 
 *
 * [Addtional info]
 *
 * @param $foo
 *   A string containing an email address.
 * @return
 *   TRUE if the address is in a valid format.
 */
function migrate_points_migrate_init() {
  return array(
    'destination' => array(
      'userpoints' => t('User Points'),
    ),
  );
}

function migrate_points_migrate_destination_delete_points($txn_id) {
  ($txn);
}

function migrate_points_migrate_destination_import_userpoints($tblinfo, $row) {
  $txn = (object)array();
    
  foreach ($tblinfo->fields as $destfield => $values) {

    if ($values['srcfield'] && $row->$values['srcfield']) {
       //dpm($destfield);
       //dpm($row->$values['srcfield']);
      $txn->$destfield = $row->$values['srcfield'];
    } else {
      $txn->$destfield = $values['default_value'];
    }
  }
    
  timer_start('prepare_userpoints');
    $errors = migrate_destination_invoke_all('prepare_userpoints', $txn, $tblinfo, $row);
  timer_stop('prepare_userpoints');
  
  
  $success = TRUE;
  foreach ($errors as $error) {
    if ($error['level'] != MIGRATE_MESSAGE_INFORMATIONAL) {
      $success = FALSE;
      break;
    }
  }
  if ($success) {
    //dpm("pre_save");
    //dpm($txn);
    timer_start('save_userpoints');
      // function expects an array
      $txn = (array)$txn;
      $ret = userpoints_userpointsapi($txn);
    timer_stop('save_userpoints');
    //dpm("post_save");
    //dpm($txn);
    if(!$ret['status']) {
      $errors[] = migrate_message($ret['reason']);
    }
    //switch back to object for consistency
    $txn = (object)$txn;
    
    // Call completion hooks, for any processing which needs to be done after node_save
    timer_start('userpoints completion hooks');
      $errors = array_merge($errors, migrate_destination_invoke_all('complete_userpoints', $txn, $tblinfo, $row));
    timer_stop('userpoints completion hooks');
    
    $sourcekey = $tblinfo->sourcekey;
    
    //map the relationship id
    db_query("INSERT INTO {" . $tblinfo->maptable . "}
                ($sourcekey, userpointsid, mcsid)
                VALUES(%d, %d, %d)",
               $row->$sourcekey, $txn->txn_id, $tblinfo->mcsid);
    
  }
  return $errors;
}


/**
 * Implementation of hook_migrate_fields().
 */
function migrate_points_migrate_destination_fields_userpoints($type) {

  $fields = array(
     'points' => t('Points'),# of points (int) (required)
     'moderate' => t('Is Moderated'), //TRUE/FALSE 
     'uid' => t('Points UID'), 
     'time_stamp' => t('Updated timestamp'), //(custom), patch added to accept this.
     'operation' => t('Operation'),//'published' 'moderated' etc.
     'tid' => t('Category ID'), 
     'expirydate' => t('Expires Timestamp'), //timestamp or 0, 0 = non-expiring; NULL = site default
     'description' => t('description'),
     'reference' => t('reference'),//reserved for module specific use
     'display' => t('display'), //whether or not to display "points awarded" message
     'txn_id' => t('Transaction ID'), //Transaction ID of points, If present an UPDATE is performed
     'entity_id' => t('Entity ID'), //ID of an entity in the Database. ex. $node->id or $user->uid
     'entity_type' => t('Entity Type') //string of the entity type. ex. 'node' or 'user' NOT 'node-content-custom'
      
    );
    //dpm($fields);
  return $fields;
}

/**
 * Implementation of hook_migrate_prepare().
 */
function migrate_points_migrate_destination_prepare_userpoints(&$ur, $tblinfo, $row) {
//not used.
}

function migrate_points_migrate_destination_types() {
  //$types = userpoints_get_categories();
  return $types;
}
