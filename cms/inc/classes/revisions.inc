<?php

/** A class for handeling revision-related actions */

abstract class revisions{

	/** This function takes care of approving revisions
	*
	* @param int|array $id - either an array of revision id's, a single revision id, or an array('element_id' => some_id, 'user_id' => some_id).
	*/
	public static function approve_revision($id){
		$data = NULL;

		if (is_array($id)) {
			if (isset($id['element_id']) && isset($id['user_id'])) {
				$data = database::sql_select_first(array('*'), 'tbl_elements_revisions', array('element_id', 'revisor'), array($id['element_id'], $id['user_id']));
			}
			else {
				$data = self::multi_approve($id);
			}
		}
		else {
			$data = database::singleton()->sql_select_first(array('*'), 'tbl_elements_revisions', array('id', 'approved'), array($id, '0'));
		}

		//only save if there is data to save
		if ($data != NULL) {

			//approve the revision if it is a single revision
			//multi revisions get approved in revisions::multi_approve()
			if (isset($data['id'])) {
				database::singleton()->sql_update_by_id($data['id'], 'tbl_elements_revisions', array('approved'), array('1'));
				$json_data = json_decode($data['data'], TRUE);
			}

			database::singleton()->sql_update_by_id($data['element_id'], 'tbl_elements', array('editor', 'edited', 'approver', 'approved', 'active_revision'), array($data['revisor'], $data['revised'], user::singleton()->id(), time(), $data['id']));
			database::singleton()->sql_update_by_id($data['element_id'], 'tbl_elements_data', array('title', 'data'), array($data['title'], $data['data']));

			search::singleton()->add_to_index($data['element_id']);
		}
	}

	/** This function arranges and approves data from multiple revisions at once
	*
	* @param array $ids - an array of revision id's.
	*/
	private static function multi_approve($ids){

		//ensure we have data with which to work
		if (is_array($ids) && !empty($ids)) {

			$merged_revision = NULL;

			//$ids is an array, and so the DB layer should use it to construct an 'OR' statement,
			//returning data for all id's at once
			$revisions = database::singleton()->sql_select(array('*'), 'tbl_elements_revisions', array('id'), array($ids), array('id'), array('DESC'));

			if ($revisions != NULL) {

				$merged_revision = array();

				//$ids is an array, and so the DB layer should use it to construct an 'OR' statement,
				//approving all id's at once
				database::singleton()->sql_update_by_id($ids, 'tbl_elements_revisions', array('approved'), array('1'));

				//construct the $data array
				//except for the 'data' field, use the data of the last revision
				//(which is first in the array because we ORDER BY `id` DESC)
				$merged_revision['id'] = $revisions[0]['id'];
				$merged_revision['element_id'] = $revisions[0]['element_id'];
				$merged_revision['title'] = $revisions[0]['title'];

				//put the merged html text into the 'data' column
				$final_data = json_decode($revisions[0]['data'], TRUE);
				$final_data['html'] = htmlentities(self::diff_data($merged_revision['element_id'], $revisions, TRUE)); //merge the diff

				$merged_revision['data'] = json_encode($final_data);
				$merged_revision['revisor'] = $revisions[0]['revisor'];
				$merged_revision['revised'] = $revisions[0]['revised'];
			}

			return $merged_revision;
		}
	}

	/** This function extracts a diff or merge of html text from an array of revisions
	*
	*  @param int $element_id - the id of the element. required to retrieve the base_text against which to diff.
	*  @param array $data_array - an array returned from 'SELECT * FROM tbl_elements_revisions'. must contain at least array(array('data' => data)).
	*  @param bool $merge - if true, will return merged text, else will return diffed text
	*
	*  @return string
	*/
	public static function diff_data($element_id, $data_array, $merge = FALSE){

		//get the base data
		$base_data = database::singleton()->sql_select_first(array('data'), 'tbl_elements_data', array('id'), array($element_id));
		if ($base_data != NULL) {
			$html = json_decode($base_data['data'], TRUE);
			$base_text = ($html != NULL && isset($html['html']))?$html['html']:'';
		}
		else {
			$base_text = '';
		}

		$merge_data = array();

		//esure we have data with which to work
		if (is_array($data_array) && !empty($data_array)) {

			//collect all the 'data' fields in preperation for the text diff/merge
			$merge_data = array();
			foreach ($data_array as $revision){
				$html = json_decode($revision['data'], TRUE);
				$merge_data[] = ($html != NULL && isset($html['html']))?$html['html']:'';
			}
			unset($revision);
		}

		return diffCompare::multi_diff($base_text, $merge_data, $merge);
	}

	/** This function returns xml lists of revision elements.
	*  The xml string will be formed as followes:
	*  <types>
	*  	 <type>
	* 		<column1 from db>...</column1 from db>
	* 		<column2 from db>...</column2 from db>
	* 		...
	* 	 </type>
	* 	 ...
	*  </types>
	*  where 'type' is substituted with the value of $type
	 *
	 * @param string $type - the type of list. this will be the root element of the returned xml string
	 * @param array $db_cols - optional. list of the columns to get from the revisions table and added to the xml.
	 * @param array|string $db_condition_fields - optional. where_fields for the db query.
	 * @param array|string $db_condition_values - optional. where_values for the db query.
	 *
	 * @return string
	 */
	public static function get_revisions_list($type, $db_cols = array('*'), $db_condition_fields = NULL, $db_condition_values = NULL){
		if ($type != NULL) {
			$list_array = database::singleton()->sql_select($db_cols, 'tbl_elements_revisions', $db_condition_fields, $db_condition_values);

			if ($list_array != NULL) {
				return xml::array_to_xml_simple($list_array, $type.'s', $type);
			}
		}
		return NULL;
	}

	private static function generate_search($input)
	{	//exchange </tag><tag> couples with spaces
		$input = preg_replace('#</\w+><\w+>#', ' ', html_entity_decode($input));
		return common::strip_non_alpha_numeric(strip_tags($input));
	}
}

?>