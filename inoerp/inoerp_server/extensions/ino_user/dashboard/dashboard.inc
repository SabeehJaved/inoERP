<<<<<<< HEAD
<?php
=======
<?php

//
$mode = 2;
$readonly = 1;
if (!empty($_GET['user_id'])) {
    $user_id = $_GET['user_id'];
} else if (!empty($_SESSION['user_id'])) {
    $user_id = $_SESSION['user_id'];
}

$ud = new user_activity_v();
// $ud->user_id = $user_id;
$result = $ud->user_contents($user_id);
$subject_noof_char = 50;
$pageno = !empty($_GET['pageno']) ? $_GET['pageno'] : 1;
$per_page = !empty($_GET['per_page']) ? $_GET['per_page'] : 10;
$query_string = !empty($_SERVER['QUERY_STRING']) ? htmlentities($_SERVER['QUERY_STRING']) : '';
$total_count = count($result);
$pagination = new pagination($pageno, $per_page, $total_count);
$pagination->setProperty('_path', 'form');
$pagination->setProperty('_query_string', $query_string);
$content_string = '<div class="table_container">';
if ($result) {
    $con_count = 0;
    if (count($result) > 0) {
        $content_string .= '<table id="forum_list" class="top_margin10 form_line_data_table"><thead> 
						 <tr class="headerBgColor">
							<th class="topics">' . gettext('Topics') . '</th>
							<th class="replies">' . gettext('Category') . '</th>
							<th class="created_by">' . gettext('Created By') . '</th>
							<th class="post_date">' . gettext('Post Date') . '</th>
						 </tr>
						</thead>';
        foreach ($result as $recod_k => $records) {
            $continue_rec = true;
            if (($recod_k > ($pageno - 1) * $per_page) && ($recod_k <= (($pageno - 1) * $per_page) + $per_page)) {
                $continue_rec = false;
            }
            if ($continue_rec) {
                $continue_rec;
            }
            $even_odd = ($con_count % 2 == 0) ? 'even' : 'odd';
            $content_string .= "<tr id=\"row_no$con_count\" class='new_row $even_odd'> "
                    . " <td class='subject_summary'>";
            $content_string .= '<a href="' . HOME_URL . 'content.php?mode=2&'
                    . 'content_id=' . $records->content_id . '&content_type_id=' . $records->content_type_id . '">';
            $content_string .= substr($records->subject, 0, $subject_noof_char);
            $content_string .= ' </a>';
            $content_string .= '</td>';
            $content_string .= '<td class="no_of_replies">';
            $content_string .= $records->category . '</td>';
            $content_string .= '<td class="created_by">';
            $content_string .= $records->username;
            $content_string .= '</td><td class="post_date">';
            $content_string .= $records->creation_date;
            $content_string .= '</td>';
            $content_string .= '</tr>';
            $con_count++;
        }
    }

    $content_string .= '</table>';
}
$content_string .= '</div>';
$content_string .= '<div id="pagination">';


$content_string .= $pagination->show_pagination();
$content_string .= '</div>';


$comment_result = $ud->user_comments($user_id);
$comment_string = '<div class="table_container">';
if (!empty($comment_result)) {
    $total_count_c = count($comment_result);
    $pagination_c = new pagination($pageno, $per_page, $total_count_c);
    $pagination_c->setProperty('_path', 'form');
    $pagination_c->setProperty('_query_string', $query_string);

    $con_count = 0;
    if (count($comment_result) > 0) {
        $comment_string .= '<table id="comment_list" class="top_margin10 form_line_data_table"><thead> 
						 <tr class="headerBgColor">
							<th class="topics">' . gettext('Subject') . '</th>
							<th class="created_by">' . gettext('Created By') . '</th>
							<th class="post_date">' . gettext('Post Date') . '</th>
						 </tr>
						</thead>';
        foreach ($comment_result as $recod_c_k => $recod_c) {
            $continue_c = true;
            if (($recod_c_k > ($pageno - 1) * $per_page) && ($recod_c_k <= (($pageno - 1) * $per_page) + $per_page)) {
                $continue_c = false;
            }
            if ($continue_c) {
                continue;
            }
            $even_odd = ($con_count % 2 == 0) ? 'even' : 'odd';
            $comment_string .= "<tr id=\"row_no$con_count\" class='new_row $even_odd'> "
                    . " <td class='subject_summary'>";
            $comment_string .= '<a href="' . HOME_URL . 'content.php?mode=2&'
                    . 'content_id=' . $recod_c->reference_id . '&content_type_id=' . $recod_c->content_type_id . '">';
            $comment_string .= substr($recod_c->comment, 0, $subject_noof_char);
            $comment_string .= ' </a>';
            $comment_string .= '</td>';
            $comment_string .= '<td class="created_by">';
            $comment_string .= $recod_c->username;
            $comment_string .= '</td><td class="post_date">';
            $comment_string .= $recod_c->creation_date;
            $comment_string .= '</td>';
            $comment_string .= '</tr>';
            $con_count++;
        }
    }

    $comment_string .= '</table>';

    $comment_string .= '</div>';
    $comment_string .= '<div id="pagination">';
    $comment_string .= $pagination_c->show_pagination();
}

$comment_string .= '</div>';

$module_icons = '<ul id="dashborad_menu">';
if (!empty($_SESSION['allowed_modules'])) {
    foreach (user_dashboard_v::$icon_stmt_a as $k => $icon_a) {
        if (in_array($k, $_SESSION['allowed_modules'])) {
            $module_icons .= '<li><k class="circle-frame"><a  class="ajax-link"  href="form.php?module_code=' . $k . '">'
                    . '<i class="fa ' . $icon_a['div_class'] . '"></i></k><j class="very-small-text">' . gettext($icon_a['description']) . '</j></a></li>';
        }
    }
}
$module_icons .= '</ul>';
?>
>>>>>>> cd19b99649c72cf8902cced52f47cd09454a45a4