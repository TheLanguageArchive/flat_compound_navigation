<?php
namespace TLA\FLAT\CompoundNavigation\Repository;

use RepositoryQuery;

/**
 * @author  Ibrahim Abdullah <ibrahim.abdullah@mpi.nl>
 * @package TLA FLAT Compound Navigation
 */
class Sparql
{
    /**
     * @var
     */
    private $sparql;

    /**
     * @var RepositoryQuery (from drupal tuque library)
     */
    private $tuque;

    /**
     * Fetching parent pid
     *
     * @param string $pid
     *
     * @return string
     */
    public function parent(string $pid): string
    {
        $query = format_string('
            SELECT DISTINCT ?field ?value
            FROM <#ri>
            WHERE {
                <info:fedora/!pid> ?field ?value;
                <info:fedora/fedora-system:def/relations-external#isConstituentOf> ?value
            }
        ', ['!pid' => $pid]);

        // let's check whether $pid is child, then fetch parent pid
        $parent    = $this->sparql()->sparqlQuery($query);
        $parentPid = $pid;

        if (count($parent) > 0 && isset($parent[0]['value']['value'])) {

            // $pid is child, getting parent pid
            $parentPid = $parent[0]['value']['value'];
        }

        return $parentPid;
    }

    /**
     * Fetching children
     *
     * @param string $pid
     *
     * @return array
     */
    public function children(string $pid)
    {
        $query = format_string('
            SELECT DISTINCT ?child
            FROM <#ri>
            WHERE {
                ?child <info:fedora/fedora-system:def/relations-external#isConstituentOf> <info:fedora/!pid> .
            }
        ', ['!pid' => $pid]);

        $results  = $this->sparql()->sparqlQuery($query);

        if (count($results) === 0) {

            // no children found
            return false;
        }

        return array_column(array_column($results, 'child'), 'value');
    }

    /**
     * Executing sparql
     *
     * @return RepositoryQuery
     */
    private function sparql()
    {
        if (null === $this->sparql) {

            module_load_include('inc', 'islandora', 'includes/utilities');
            $tuque = islandora_get_tuque_connection();

            $this->sparql = $tuque->repository->ri;
        }

        return $this->sparql;
    }
}
