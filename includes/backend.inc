<?php
/**
 * Adding compound query backend that allows disabling compound querying
 *
 * @return array
 */
function flat_compound_navigation_islandora_compound_object_query_backends() {

    $module_path = drupal_get_path('module', 'flat_compound_navigation');

    return [

        'flat_compound_navigation_query_backend' => [

            'title' => t('FLAT Compound Navigation - this backend disables querying for compound navigation'),
            'callable' => 'flat_compound_navigation_compound_object_query',
        ],
    ];
}

/**
 * disabling islandora compound object querying
 */
function flat_compound_navigation_compound_object_query() {
    return [];
}
