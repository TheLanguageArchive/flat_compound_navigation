<?php

/**
 * @file
 * flat-compound-navigation-sidebar.tpl.php
 *
 * $label - Title of compound object
 * $count - Count of objects in compound object
 * $uri - URL to manage compound object
 * $previous - PID of previous object in sequence or blank if on first
 * $next - PID of next object in sequence or blank if on last
 * $children - array of objects in compound object
 */

?>
 <div class="islandora-compound-prev-next">
 <span class="islandora-compound-title"><?php
  print t('Part of: @parent (@count @objects)', array('@parent' => $label, '@count' => $count, '@objects' => format_plural($count, 'object', 'objects'))); ?>
 <?php if ($uri): ?>
    <?php print l(t('manage parent'), $uri); ?>
 <?php endif; ?>
 </span><br/>

 <?php if (false !== $previous): ?>
   <?php print l(t('Previous'), 'islandora/object/' . $previous); ?>
 <?php endif; ?>

 <?php if (false != $next): ?>
   <?php print l(t('Next'), 'islandora/object/' . $next); ?>
 <?php endif; ?>

 <?php if ($count > 0): ?>
   <?php $query_params = drupal_get_query_parameters(); ?>
   <div class="islandora-compound-thumbs">
   <?php foreach ($children as $child): ?>
     <div class="islandora-compound-thumb">
     <span class="islandora-compound-caption"><?php print $child['label'];?></span>
     <?php print l(
       flat_compound_navigation_image_tag($child, $active),
       'islandora/object/' . $child['pid'],
       array('html' => TRUE, 'query' => $query_params)
     );?>
     <?php echo theme('flat_access_level_display_display_label', ['roles' => $child['roles']]); ?>
     </div>
   <?php endforeach; // foreach $children ?>
   </div> <!-- // islandora-compound-thumbs -->
 <?php endif; // $count > 0 ?>
 </div>
