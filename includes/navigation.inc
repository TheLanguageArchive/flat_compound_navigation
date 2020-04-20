<?php
namespace TLA\FLAT\CompoundNavigation;

use TLA\FLAT\CompoundNavigation\Repository\Sparql;
use TLA\FLAT\CompoundNavigation\Repository\Solr;

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
        $pids   = $this->sparql->children($parent);

        if (false === $pids) {

            // no children found
            return false;
        }

        $pids[] = $parent;
        $data   = $this->solr->fetch($pids);

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

        // determining prev/next by copying raw data
        $indexed = $data['results'];

        // removing parent from solr results
        unset($indexed[$parent]);

        // reindexing raw data (indexed by pid) to int based
        $children = array_keys($indexed);

        $current  = $pid === $parent ? 0 : array_search($pid, $children);
        $previous = $children[$current - 1] ?? false;
        $next     = $children[$current + 1] ?? false;

        return [

            'label'    => $data['results'][$parent]['label'],
            'count'    => $data['count'] - 1, // removing parent from count
            'uri'      => url('islandora/object/' . $parent . '/manage'),
            'active'   => $pid,
            'previous' => $previous,
            'next'     => $next,
            'children' => $indexed,
        ];
    }
}
