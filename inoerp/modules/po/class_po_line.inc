<?php

 class po_line extends dbObject {

  public static $table_name = "po_line";
  public static $primary_column = "po_line_id";
  public static $parent_primary_column = "po_header_id";
  public static $key_column = 'line_number';
  public static $module = "po";
  public $checkbox = [
   "rev_enabled_cb",
   'hold_cb'
  ];
  public $field_a = [
   'po_line_id',
   'po_header_id',
   'line_type',
   'line_number',
   'bpa_line_id',
   'receving_org_id',
   'item_id_m',
   'item_description',
   'line_description',
   'uom_id',
   'exchange_rate',
   'line_quantity',
   'price_list_header_id',
   'price_date',
   'unit_price',
   'line_price',
   'reference_doc_type',
   'reference_doc_number',
   'hold_cb',
   "ef_id",
   "created_by",
   "creation_date",
   "last_update_by",
   "last_update_date"
  ];
  public $initial_search = [
   "po_header_id",
   "po_number",
   "item_id_m",
   "item_number",
   "item_description"
  ];
  public $column = [
   "po_header_id",
   "po_number",
   "item_id_m",
   "item_number",
   "item_description"
  ];
  public $numberField = [
   'exchange_rate',
   'line_quantity',
   'unit_price',
   'line_price',
  ];
  public $requiredField = [
   'po_header_id',
   'line_number',
   'item_description'
  ];
  public $fields_inForm_notInDataBase = [
   "po_number",
   "item_number",
   'released_quantity',
   'released_amount',
   'agreed_quantity',
   'agreed_amount',
  ];
  public $fields_inHeader_needsToBeInserted_inPOST = [
   "po_header_id"
  ];
  public $po_line_id;
  public $po_header_id;
  public $line_type;
  public $bpa_line_id;
  public $line_number;
  public $receving_org_id;
  public $item_id_m;
  public $item_description;
  public $line_description;
  public $uom_id;
  public $line_quantity;
  public $unit_price;
  public $line_price;
  public $agreed_quantity;
  public $agreed_amount;
  public $price_list_header_id;
  public $price_date;
  public $reference_doc_type;
  public $reference_doc_number;
  public $ef_id;
  public $po_status;
  public $rev_enabled_cb;
  public $rev_number;
  public $hold_cb;
  public $created_by;
  public $creation_date;
  public $last_update_by;
  public $last_update_date;
  public $time;
  public $msg;
  public $item_number;
  public $exchange_rate;
  public $released_quantity;
  public $released_amount;

  public function _before_save() {

   //change the PO status to Requires reapproval when Qty to Price is changed
   if (!empty($this->po_line_id)) {
    $existing_line_data = new po_line();
    $existing_line_data->findBy_id($this->po_line_id);
    $po_header_data = new po_header();
    $po_header_data->findBy_id($existing_line_data->po_header_id);
    if ($po_header_data->po_status == 'APPROVED') {
     if (($this->line_quantity != $existing_line_data->line_quantity) || ($this->unit_price != $existing_line_data->unit_price) || ($this->line_price != $existing_line_data->line_price)) {
      $po_header_data->po_status = 'REAPPROVAL';
      $po_header_data->audit_trial();
      $po_header_data->save();
     }
    }
    //put on hold if released amount / quantity is more tha blaket quantity
    if ($po_header_data->po_type == 'BLANKET_RELEASE') {
     $this->_update_hold();
    }
   } else {
    $existing_line_data = po_line::find_by_parent_id($this->po_header_id);
    $po_header_data = new po_header();
    $po_header_data->findBy_id($this->po_header_id);
    if ($po_header_data->po_status == 'APPROVED') {
     if ($existing_line_data) {
      $po_header_data->po_header_id = $this->po_header_id;
      $po_header_data->po_status = 'REAPPROVAL';
      $po_header_data->audit_trial();
      $po_header_data->save();
     }
    }
   }
  }

  public function _after_save() {
   
  }

  private function _update_hold() {
   $hold_value = 0;
   $hold_ref = new sys_hold_reference();
   $hold_ref->reference_table = 'po_line';
   $hold_ref->reference_id = $this->po_line_id;
   $agreement_details = self::find_agreement_details_by_lineId($this->bpa_line_id);
//   pa($agreement_details);
   $hold_ref->hold_code = 'PO_LINE_OVER_RELEASED_QTY';
   if (($agreement_details) && ($agreement_details->released_quantity + $this->line_quantity) > $agreement_details->agreed_quantity) {
    if (!$hold_ref->findBy_referenceTableId_holdCode(true)) {
     $application_reason = 'Released quantity is more than aggreed quantity';
     $this->_put_onHold('PO_LINE_OVER_RELEASED_QTY', $application_reason);
    }
    $hold_value = 1;
   }

   if (($this->hold_cb == 1) && ($hold_value == 0) && !empty($this->po_line_id)) {
    $hold_ref->findBy_referenceTableId_holdCode(true);
    if (!empty($hold_ref->sys_hold_reference_id)) {
     $hold_ref->removal_reason = 'Quantity variance is removed';
     $this->_remove_Hold($hold_ref);
     $hold_value = 0;
    }
   }

   $hold_ref->hold_code = 'PO_LINE_OVER_RELEASED_AMNT';
   if (($agreement_details) && ($agreement_details->released_amount + $this->line_price) > $agreement_details->agreed_amount) {
    if (!$hold_ref->findBy_referenceTableId_holdCode(true)) {
     $application_reason = 'Released amount is more than aggreed amount';
     $this->_put_onHold('PO_LINE_OVER_RELEASED_AMNT', $application_reason);
    }
    $hold_value = 1;
   }

   if (($this->hold_cb == 1) && ($hold_value == 0) && !empty($this->po_line_id)) {
    $hold_ref->findBy_referenceTableId_holdCode(true);
    if (!empty($hold_ref->sys_hold_reference_id)) {
     $hold_ref->removal_reason = 'Amount variance is removed';
     $this->_remove_Hold($hold_ref);
     $hold_value = 0;
    }
   }
   $this->hold_cb = $hold_value;
  }

  private function _remove_Hold(&$obj) {
   $obj->hold_removed_by = $_SESSION['username'];
   $obj->hold_removed_on = current_time(true);
   $obj->save();
  }

  private function _put_onHold($hold_code, $application_reason) {
   $hold_ref = new sys_hold_reference();
   $hold_ref->hold_code = $hold_code;
   $hold_ref->reference_table = 'po_line';
   $hold_ref->reference_id = $this->po_line_id;
   $hold_ref->hold_applied_by = $_SESSION['username'];
   $hold_ref->hold_applied_on = current_time(true);
   $hold_ref->application_reason = $application_reason;
   $hold_ref->save();
  }

  public function _before_delete() {
   $detail_line_exists = po_detail::find_by_parent_id($this->po_header_id);
   if ($detail_line_exists) {
    echo "<br>Delete is not allowed as PO Detail Lines exists!. First delete detail lines and then try again ";
    return -99;
   } else {
    return 1;
   }
  }

  public static function find_agreement_details_by_lineId($bpa_line_id) {
   $sql = "SELECT 
          bpa.po_number as bpa_number,rd.ref_po_header_id, rd.released_quantity,
          rd.released_amount, rd.item_id_m, bpa_line.line_quantity as agreed_quantity,
          bpa_line.line_price as agreed_amount, bpa_line.po_line_id as bpa_line_id
          from po_header bpa,
          po_line bpa_line,
          (
           SELECT ph.ref_po_header_id, SUM(line_quantity) as released_quantity,
          SUM(line_price) as released_amount, item_id_m
          FROM po_line pl,
          po_header ph
          WHERE ph.po_header_id = pl.po_header_id
          AND ph.po_type = 'BLANKET_RELEASE'
          GROUP BY ph.ref_po_header_id, pl.item_id_m
              ) as rd

          WHERE    rd.ref_po_header_id =  bpa.po_header_id
          AND bpa.po_header_id = bpa_line.po_header_id
          AND bpa_line.item_id_m = rd.item_id_m
          AND bpa_line.po_line_id = '{$bpa_line_id}'
           LIMIT 1
";
   $result = self::find_by_sql($sql);
//   pa($result);
//   echo "<br>going out $sql";
   return !empty($result) ? array_pop($result) : false;
  }

  Public static function po_line_types() {
   $option_header = option_header::find_by_name('PO_LINE_TYPE');
   $po_types = option_line::find_by_option_id($option_header->option_header_id);
   return $po_types;
  }

  public static function find_by_po_headerId($po_header_id) {
   $sql = "SELECT * FROM " . self::$table_name . " where po_header_id = '{$po_header_id}'";
   $result_array = self::find_by_sql($sql);
   return $result_array;
  }

  public static function find_by_poHeaderId_itemId($po_header_id, $item_id) {
   $sql = "SELECT * FROM " . self::$table_name . " where po_header_id = '{$po_header_id}' AND item_id_m = '{$item_id}' LIMIT 1 ";
   $result = self::find_by_sql($sql);
   return !empty($result) ? array_pop($result) : false;
  }

 }

//end of po_line class
?>