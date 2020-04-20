<?php
namespace TLA\FLAT\CompoundNavigation\Repository;

use Apache_Solr_Service;
use Exception;

/**
 * Solr repository
 *
 * @author  Ibrahim Abdullah <ibrahim.abdullah@mpi.nl>
 * @package TLA FLAT Compound Navigation
 */
class Solr
{
    /**
     * @var Apache_Solr_Service
     */
    private $solr;

    /**
     * Connect to apache solr
     *
     * @return Apache_Solr_Service
     */
    public function solr()
    {
        if (null !== $this->solr) {
            return $this->solr;
        }

        $path_parts = parse_url(variable_get('islandora_solr_url', 'localhost:8080/solr'));

        $this->solr = new Apache_Solr_Service($path_parts['host'], $path_parts['port'], $path_parts['path'] . '/');
        $this->solr->setCreateDocuments(0);

        return $this->solr;
    }

    /**
     * Fetching navigation data from solr
     *
     * @param array $pids
     *
     * @return array|false
     */
    public function fetch(string $parent)
    {
        $params = $this->params($parent);
        $data   = $this->query($params);

        if (false === $data || $data['response']['numFound'] <= 1) {
            return false;
        }

        $results = [];

        foreach ($data['response']['docs'] as $row) {

            $results[$row['PID']] = [

                'pid'     => $row['PID'],
                'label'   => $row['fgs_label_s'],
                // this flag determines whether current user can see OBJ datastream
                'allowed' => false,
                // compound does not have roles, users indexed
                'roles'   => $row['policy_datastream_roles_ms'] ?? [],
                'users'   => $row['policy_datastream_users_ms'] ?? [],

            ];
        }

        return [

            'count'   => $data['response']['numFound'],
            'results' => $results,
        ];
    }

    /**
     * Running solr query
     *
     * @param array $params
     *
     * @return array|bool
     */
    private function query(array $params)
    {
        try {

            $results = $this->solr()->search('*:*', 0, 1000, $params);

        } catch (Exception $e) {

            drupal_set_message(check_plain(t('Error searching Solr index')) . ' ' . $e->getMessage(), 'error');
            return false;
        }

        return json_decode($results->getRawResponse(), true);
    }

    /**
     * Preparing params
     *
     * @param string $parent
     *
     * @return array
     */
    private function params(string $parent): array
    {
        global $user;

        $params = [

            'sort' => 'fgs_label_ss asc',
            'fl'   => 'PID, fgs_label_s, policy_datastream_roles_ms, policy_datastream_users_ms',
            'fq'   => [
                format_string('({!terms f=RELS_EXT_isConstituentOf_uri_ms}!parent) OR ({!terms f=PID}!pid)', ['!parent' => ('info:fedora/' . $parent), '!pid' => $parent]),
            ],
        ];

        return $params;
    }
}
