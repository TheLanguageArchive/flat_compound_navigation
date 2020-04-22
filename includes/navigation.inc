<?php
namespace TLA\FLAT\CompoundNavigation;

use TLA\FLAT\CompoundNavigation\Repository\Sparql;
use TLA\FLAT\CompoundNavigation\Repository\Solr;

use function islandora_object_load;
use function islandora_object_manage_access_callback;

module_load_include('inc', 'flat_compound_navigation', 'includes/repositories/sparql');
module_load_include('inc', 'flat_compound_navigation', 'includes/repositories/solr');

/**
 * Navigation service
 *
 * @author  Ibrahim Abdullah <ibrahim.abdullah@mpi.nl>
 * @package TLA FLAT Compound Navigation
 */
class NavigationService
{
    /**
     * @var Sparql
     */
    private $sparql;

    /**
     * @var Solr
     */
    private $solr;

    /**
     * Constructor
     */
    public function __construct()
    {
        $this->sparql = new Sparql();
        $this->solr   = new Solr();
    }

    /**
     * Fetching navigation data
     *
     * @param string $pid
     *
     * @return array
     */
    public function fetch(string $pid)
    {
        $parent = $this->sparql->parent($pid);
        $data   = $this->solr->fetch($parent);

        if (false === $data) {
            return false;
        }

        return $this->normalize($pid, $parent, $data);
    }

    /**
     * Normalizing data
     *
     * @param string $parent
     * @param array  $data
     *
     * @return array|false
     */
    private function normalize(string $pid, string $parent, array $data)
    {
        if (!isset($data['results'][$parent])) {
            return false;
        }

        global $user;

        foreach ($data['results'] as $id => $row) {

            // $user->roles is indexed by role id
            $allowed = array_intersect($row['roles'], array_values($user->roles));
            if (count($allowed) > 0) {

                // user has role that allows viewing OBJ
                $data['results'][$id]['allowed'] = true;

            } elseif (in_array($user->name, $row['users'])) {

                // user is allowed to view OBJ
                $data['results'][$id]['allowed'] = true;
            }
        }

        // determining prev/next by copying raw data
        $indexed = $data['results'];

        // removing parent from solr results
        unset($indexed[$parent]);

        // reindexing raw data (indexed by pid) to int based
        $children = array_keys($indexed);

        $current  = $pid === $parent ? 0 : array_search($pid, $children);
        $previous = $children[$current - 1] ?? false;
        $next     = $children[$current + 1] ?? false;

        // checking parent access
        $object = islandora_object_load($parent);
        $manage = islandora_object_manage_access_callback([
            ISLANDORA_METADATA_EDIT, ISLANDORA_MANAGE_PROPERTIES, ISLANDORA_ADD_DS
        ], $object);

        return [

            'label'    => $data['results'][$parent]['label'],
            'count'    => $data['count'] - 1, // removing parent from count
            'uri'      => true === $manage ? url('islandora/object/' . $parent . '/manage') : false,
            'active'   => $pid,
            'previous' => $previous,
            'next'     => $next,
            'children' => $indexed,
        ];
    }
}
