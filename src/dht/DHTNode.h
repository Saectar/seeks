/**
 * This is the p2p messaging component of the Seeks project,
 * a collaborative websearch overlay network.
 *
 * Copyright (C) 2006, 2010  Emmanuel Benazera, juban@free.fr
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DHTNODE_H
#define DHTNODE_H

#include "stl_hash.h"
#include "dht_configuration.h"
#include "DHTVirtualNode.h"
#include "Stabilizer.h"

namespace dht
{
  class Transport;

  class DHTNode
  {
    public:
      /**
       * \brief constructor.
       * @param net_addr address of the DHT node server.
       * @param net_port port the DHT node server is listening on.
       * @param start_dht_node whether to start the node, i.e. its server
       *        and automated stabilization system.
       */
      DHTNode(const char *net_addr, const short &net_port=0,
              const bool &start_dht_node=true,
              const std::string &vnodes_table_file="vnodes-table.dat");

      /**
       * \brief destructor.
       */
      ~DHTNode();

      /**
       * \brief start DHT node.
       */
      void start_node();

      /**
       * \brief create virtual nodes.
       */
      void create_vnodes();

      /**
       * \brief create virtual node.
       */
      virtual DHTVirtualNode* create_vnode();
      virtual DHTVirtualNode* create_vnode(const DHTKey &idkey,
                                           LocationTable *lt);

      /**
       * \brief init servers.
       */
      virtual void init_server();

      /**
       * \brief resets data and structures that are dependent on the virtual nodes.
       */
      virtual void reset_vnodes_dependent() {};

      /**
       * \brief stops DHT node.
       */
      void stop_node();

      /**
       * \brief destroy all virtual nodes on this DHT node.
       */
      void destroy_vnodes();

      /*- persistence. -*/
      /**
       * \brief loads table of virtual nodes and location tables from persistent
       * data, if it exists.
       */
      bool load_vnodes_table() throw (dht_exception);

      /**
       * \brief loads deserialized vnodes and tables into memory structures.
       */
      void load_vnodes_and_tables(const std::vector<const DHTKey*> &vnode_ids,
                                  const std::vector<LocationTable*> &vnode_ltables);

      /**
       * \brief makes critical data (vnode keys and location tables) persistent.
       */
      bool hibernate_vnodes_table() throw (dht_exception);

      /*- main functions -*/
      /**
       * \brief DHTNode key generation, one per virtual node.
       */
      static DHTKey generate_uniform_key();

      /**
       * \brief join at startup.
       * tries from the list in configuration file and with the LocationTable information if any,
       * and if reset is not true.
       */
      dht_err join_start(const std::vector<NetAddress> &bootstrap_nodelist,
                         const bool &reset);
      /**
       * \brief self-boostrap.
       * Bootstraps itself by building a circle of its virtual nodes.
       * Useful only for the first node of a circle.
       */
      void self_bootstrap();

      /**
       * \brief nodes voluntarily leave the circle, announces it
       *        to its predecessor and successor.
       */
      dht_err leave() const;


    public:
      void estimate_nodes(const unsigned long &nnodes,
                          const unsigned long &nnvnodes);

      bool on_ring_of_virtual_nodes();

    public:
      /**
       * accessors.
       */
      NetAddress getNetAddress() const
      {
        return _l1_na;
      }

      /**----------------------------**/
      /**
       * Main routines using RPCs.
       */

      /**
       * \brief joins the circle by asking dk for the successor to its own key.
       *        This is done for _all_ the virtual nodes of a peer.
       * @param na_bootstrap network address of a bootstrap node (any known peer).
       * @param dk_bootstrap DHT key of the bootstrap node.
       * @return status.
       */
      dht_err join(const NetAddress& dk_bootstrap_na,
                   const DHTKey &dk_bootstrap);

    public:
      /**
       * configuration.
       */
      static std::string _dht_config_filename;

      /**
       * this peer net address.
       */
      NetAddress _l1_na;

      /**
       * estimate of the number of peers on the circle.
       */
      int _nnodes;
      int _nnvnodes;

      Transport *_transport;

      /**
       * node's stabilizer.
       */
      Stabilizer* _stabilizer;

      /**
       * persistent table of virtual nodes and location tables, in a file.
       */
      std::string _vnodes_table_file;

      /**
       * whether this node is connected to the ring
       * (i.e. at least one of its virtual nodes is).
       */
      bool _connected;

      /**
       * whether this node is running on persistent data
       * (virtual node keys that were created and left by
       * another object, another run).
       */
      bool _has_persistent_data;
  };

} /* end of namespace. */

#endif
