<?php
/**
 * @file
 * comstack.forms.inc
 */

/**
 * Form building function which provides the add/edit form for
 * the comstack_message_type entity.
 */
function comstack_message_type_form($form, &$form_state, $comstack_message_type, $op = 'edit', $entity_type = NULL) {
  if ($op === 'clone') {
    $comstack_message_type->label .= ' (cloned)';
    $comstack_message_type->type = $entity_type . '_clone';
  }

  // Check for existing content.
  $content_count = 0;
  if ($op != 'add' && $op != 'clone') {
    $content_count = db_query('SELECT COUNT(*) FROM {comstack_message} WHERE type = :type', array(':type' => $comstack_message_type->type))->fetchField();

    if ($content_count > 0) {
      $form['existing_content'] = array('#markup' => t('<p><strong>Note:</strong> You cannot delete this type as messages of this type exist. Please remove the content first.</p>'));
    }
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#default_value' => isset($comstack_message_type->label) ? $comstack_message_type->label : '',
  );

  // Machine-readable type name.
  $form['type'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($comstack_message_type->type) ? $comstack_message_type->type : '',
    '#machine_name' => array(
      'exists' => 'comstack_get_message_types',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this thread type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $destinations = comstack_get_destinations_flat();

  $form['destination_type'] = array(
    '#type' => 'select',
    '#title' => t('Destination type'),
    '#options' => $destinations,
    '#default_value' => isset($comstack_message_type->destination_type) ? $comstack_message_type->destination_type : '',
    '#required' => TRUE,
  );

  $form['destination_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('User destination specific settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#states' => array(
      'visible' => array(
        ':input[id="edit-destination-type"]' => array('value' => 'user'),
      ),
    ),
  );

  $form['destination_settings']['structure'] = array(
    '#title' => t('Type structure'),
    '#type' => 'select',
    '#options' => array(
      COMSTACK_TYPE_STRUCTURE_FLAT => t('Flat'),
      COMSTACK_TYPE_STRUCTURE_CONVERSATION => t('Conversation'),
    ),
    '#default_value' => isset($comstack_message_type->structure) ? $comstack_message_type->structure : COMSTACK_TYPE_STRUCTURE_FLAT,
    '#description' => t("The type structure, Flat would constitute single messages that don't represent an ongoing dialogue. Conversation structure is the opposite, in addition to message entities conversation entities are created to wrapper and collect messages within."),
  );

  $form['destination_settings']['user_preferences'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow user preferences for this message type'),
    '#default_value' => isset($comstack_message_type->user_preferences) ? $comstack_message_type->user_preferences : 0,
    '#description' => t('This option will allow users to select which delivery methods they get messages of this type by (from the chosen set in this form).'),
  );

  $form['destination_settings']['implement_perms'] = array(
    '#type' => 'checkbox',
    '#title' => t('Implement specific permissions for messages of this type'),
    '#default_value' => isset($comstack_message_type->implement_perms) ? $comstack_message_type->implement_perms : 0,
    '#description' => t('Check to provide CRUD type permissions for interacting with conversations (if the structure is set so) and/or messages of this type.'),
  );

  $delivery_methods = comstack_get_delivery_methods_flat();

  $form['delivery_methods'] = array(
    '#title' => t('Delivery methods to allow'),
    '#type' => 'checkboxes',
    '#required' => TRUE,
    '#options' => $delivery_methods,
    '#default_value' => isset($comstack_message_type->delivery_methods) ? $comstack_message_type->delivery_methods : array(),
    '#tree' => TRUE,
  );

  $form['store_last_read_time'] = array(
    '#type' => 'checkbox',
    '#title' => t('Store last read time'),
    '#default_value' => isset($comstack_message_type->store_last_read_time) ? $comstack_message_type->store_last_read_time : 0,
    '#description' => t('If checked message metadata will be updated storing read information.'),
  );

  $form['record_read_receipts'] = array(
    '#type' => 'checkbox',
    '#title' => t('Record read receipts'),
    '#default_value' => isset($comstack_message_type->record_read_receipts) ? $comstack_message_type->record_read_receipts : 0,
    '#description' => t('If checked detailed records of who has read a message and when will be kept. This allows for "Read at xx:xx" inline information.'),
  );

  $form['record_read_receipts_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Grouping settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#states' => array(
      'visible' => array(
        ':input[id="edit-record-read-receipts"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['record_read_receipts_perm'] = array(
    '#type' => 'checkbox',
    '#title' => t('Provide read receipts permissions'),
    '#default_value' => isset($comstack_message_type->record_read_receipts_perm) ? $comstack_message_type->record_read_receipts_perm : 0,
    '#description' => t('Provide a new permission that will allow individual users to opt out of sending read receipts.'),
  );

  $form['record_read_receipts_default'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable read receipts by default?'),
    '#default_value' => isset($comstack_message_type->record_read_receipts_default) ? $comstack_message_type->record_read_receipts_default : 1,
    '#description' => t('Provide a new permission that will allow individual users to opt out of sending read receipts.'),
  );

  $form['grouping'] = array(
    '#type' => 'checkbox',
    '#title' => t('Group messages of this type'),
    '#default_value' => isset($comstack_message_type->grouping) ? $comstack_message_type->grouping : 0,
    '#description' => t('Check if the messages in this type can be grouped together, e.g. Notifications type with the groups: Photo, Note, Comment reply.'),
  );

  $form['grouping_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Grouping settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#states' => array(
      'visible' => array(
        ':input[id="edit-grouping"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['grouping_fieldset']['delivery_methods_per_grouping'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow delivery methods per grouping?'),
    '#default_value' => isset($comstack_message_type->delivery_methods_per_grouping) ? $comstack_message_type->delivery_methods_per_grouping : 0,
    '#description' => t('Check to enable the definition of which delivery methods are used per grouping.'),
  );

  $form['grouping_fieldset']['groups'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Groups'),
    '#options' => array(),
    '#default_value' => isset($comstack_message_type->groups) ? $comstack_message_type->groups : array(),
    '#tree' => TRUE,
  );

  $form['subject'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Subjects'),
    '#default_value' => isset($comstack_message_type->subject) ? $comstack_message_type->subject : 0,
    '#description' => t('Check if the messages in this type can be have a subject.'),
  );

  $form['template'] = array(
    '#type' => 'checkbox',
    '#title' => t('Messages of this type should follow a template'),
    '#default_value' => isset($comstack_message_type->template) ? $comstack_message_type->template : 0,
    '#description' => t('Check if a structured text template should be used, left unchecked the messages shall be free-form/text.'),
  );

  $form['template_text'] = array(
    '#type' => 'textarea',
    '#title' => t('Template text'),
    '#default_value' => isset($comstack_message_type->template_text) ? $comstack_message_type->template_text : '',
    '#description' => t('Template follows Drupal t() conventions... should be tokens really'),
    '#states' => array(
      'visible' => array(
        ':input[id="edit-template"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['actions'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable actions'),
    '#default_value' => isset($comstack_message_type->actions) ? $comstack_message_type->actions : 0,
    '#description' => t('When enabled actions will be requested via a hook per message, e.g. Dimiss, Reply.'),
  );

  $form['flood_control'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable flood control'),
    '#default_value' => isset($comstack_message_type->flood_control) ? $comstack_message_type->flood_control : 0,
    '#description' => t('Enable flood control to rate limit the number of messages of this type that can be sent per user.'),
  );

  $form['flood'] = array(
    '#type' => 'fieldset',
    '#title' => t('Flood settings'),
    '#states' => array(
      'visible' => array(
        ':input[id="edit-flood-control"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['flood']['flood_control_interval'] = array(
    '#title' => t('Time interval'),
    '#type' => 'select',
    '#options' => drupal_map_assoc(array(60, 180, 300, 600, 900, 1800, 2700, 3600, 10800, 21600, 32400, 43200, 86400), 'format_interval'),
    '#default_value' => isset($comstack_message_type->flood_control_interval) ? $comstack_message_type->flood_control_interval : 3600,
  );

  $form['flood']['flood_control_limit'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum number of reports allowed within the interval'),
    '#required' => TRUE,
    '#default_value' => isset($comstack_message_type->flood_control_limit) ? $comstack_message_type->flood_control_limit : 50,
    '#element_validate' => array(
      'element_validate_integer_positive',
    ),
  );

  $form['defer_send_to_cron_queue'] = array(
    '#type' => 'checkbox',
    '#title' => t('Messages should be added to a queue which will be processed on Cron'),
    '#default_value' => isset($comstack_message_type->defer_send_to_cron_queue) ? $comstack_message_type->defer_send_to_cron_queue : 0,
    '#description' => t("Processing via Cron, whilst slower could be more performant if you're expecting to send a large volume of messages. If unchecked messages are sent instantly."),
  );

  $form['cron_purge_active'] = array(
    '#type' => 'checkbox',
    '#title' => t('Purge read messages of this type via Cron'),
    '#default_value' => isset($comstack_message_type->cron_purge_active) ? $comstack_message_type->cron_purge_active : 0,
    '#description' => t('This functionality requires that Cron is configured correctly.'),
  );

  $form['cron_purge'] = array(
    '#type' => 'fieldset',
    '#title' => t('Message purge settings'),
    '#states' => array(
      'visible' => array(
        ':input[id="edit-cron-purge-active"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['cron_purge']['cron_purge_interval'] = array(
    '#title' => t('Time interval'),
    '#type' => 'select',
    '#options' => drupal_map_assoc(array(3600, 10800, 21600, 32400, 43200, 86400, 259200, 432000, 604800, 1209600, 2592000), 'format_interval'),
    '#default_value' => isset($comstack_message_type->cron_purge_interval) ? $comstack_message_type->cron_purge_interval : 604800,
  );


  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save message type'),
    '#weight' => 40,
  );

  if ($op != 'add' && $op != 'clone' && $content_count < 1) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete message type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('comstack_message_type_form_submit_delete')
    );
  }

  return $form;
}

/**
 * Form API submit callback for the type form.
 */
function comstack_message_type_form_submit(&$form, &$form_state) {
  $comstack_message_type = entity_ui_form_submit_build_entity($form, $form_state);

  // Save the entity!
  entity_save('comstack_message_type', $comstack_message_type);

  // Redirect the user.
  $form_state['redirect'] = 'admin/structure/comstack/types';
}

/**
 * Form API submit callback for the delete button.
 */
function comstack_message_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/comstack/types/' . $form_state['comstack_message_type']->type . '/delete';
}
