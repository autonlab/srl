/*
   Date:         February 18, 2015
   Author(s):    Anthony Wertz
   Copyright (c) Carnegie Mellon University
*/
#ifndef __SRL_Controller_hpp__
#define __SRL_Controller_hpp__

#include "concurrent/Thread.hpp"
#include "concurrent/UniqueQueue.hpp"
#include "concurrent/Time.hpp"

#include "srl/Connection.hpp"
#include "srl/CommunicationInterface.hpp"
#include "srl/ServiceProvider.hpp"
#include "srl/Router.hpp"
#include "srl/BuiltinMessageFactory.hpp"
#include "srl/Log.hpp"

#include <list>
#include <map>

namespace al { namespace srl
{
    /*!
     * This class implements a thread that handles connections and services. It
     * also controls the message routers.
     */
    class Controller : public concurrent::Thread
    {
        public:
            class ConnectionDescriptor
            {
                public:
                    ConnectionDescriptor( void ) :
                        id(-1), connection(0), expiration(1), processing(false) { }

                    ConnectionDescriptor(
                            const int id,
                            Connection * const connection,
                            const long expiration ) :
                        id(id), connection(connection),
                        expiration(expiration), processing(false) { }

                    bool is_expired( const long current_time ) const
                        { return (expiration > 0) && (current_time > expiration); }

                    int get_id( void ) const { return id; }

                    Connection * get_connection( void ) const { return connection; }

                    long get_expiration( void ) const { return expiration; }
                    void set_expiration( const long expiration ) { this->expiration = expiration; }

                    bool is_being_processed( void ) const { return processing; }
                    void set_processing( const bool flag ) { processing = flag; }

                private:
                    int             id;
                    Connection *    connection;
                    long            expiration;
                    bool            processing;
            };

        public:
            static const long DefaultIdleConnectionTimeout = 60*10; // Seconds. (10 minutes.)

        public:
            Controller( void ) :
                next_client_id(100), connection_timeout(DefaultIdleConnectionTimeout) { }

            /*!
             * Register an interface with the controller.
             *
             * @param cif The interface to register.
             * @param managed If true, the controller will destroy the interface at shutdown.
             * @return True if successfully registered, false otherwise.
             */
            bool register_interface( CommIF * const cif, const bool managed = true )
            {
                if (!cif) return false;
                comm_ifs.push_back(CommIFDescriptor(cif, managed));
            }

            /*!
             * Add a connection to the pool.
             *
             * @param connection The connection to add.
             * @param expiration The connection expiration time. If zero, it will be calculated
             *          using the current time. If negative, the connection will never expire.
             */
            void add_connection( Connection * connection, long expiration = 0 )
            {
                if (expiration == 0) expiration = concurrent::stime() + connection_timeout;
                connections.insert(
                        std::pair<int, ConnectionDescriptor>(
                            next_client_id,
                            ConnectionDescriptor(next_client_id, connection, expiration)));
                ++next_client_id;
            }

            /*!
             * Find a client connection by ID.
             * 
             * @param The client connection ID.
             * @return A pointer to the client's connection, or false if not found.
             */
            ConnectionDescriptor * get_connection( const int id )
            {
                auto elem = connections.find(id);
                return elem != connections.end() ? &(elem->second) : NULL;
            }

            /*!
             * Get the queue of active connections. These are the connections which have some
             * data available to be read and processed.
             *
             * @return The thread-safe queue containing the connections.
             */
            concurrent::UniqueQueue<ConnectionDescriptor *> & get_active_connections( void )
                { return active_connections; }

            /*!
             * Get the provider interface by specifying the module name.
             *
             * @param name The name of the module.
             * @return A pointer to the provider interface if registered, NULL otherwise.
             */
            ServiceProvider * get_provider( const std::string name )
            {
                auto provider_iter = providers.find(name);
                return provider_iter != providers.end() ? provider_iter->second : 0;
            }

            /*!
             * Register a provider with the controller.
             *
             * @param provider The provider interface to register.
             */
            void register_provider( ServiceProvider * const provider )
            {
                provider_connection_map[provider->get_connection()] = provider->get_name();
                providers[provider->get_name()] = provider;
            }

            /*!
             * Unregister a provider.
             *
             * @param provider_name The name of the provider.
             */
            void unregister_provider( const std::string provider_name, const std::string reason = "" )
            {
                auto provider_iter = providers.find(provider_name);
                if (provider_iter != providers.end())
                {
                    ServiceProvider * provider = provider_iter->second;
                    
                    // Tell the provider it is being disconnected.
                    if (provider->get_connection()->is_connected())
                        provider->get_connection()->send(DisconnectMessageFactory::generate(reason));
                
                    // Remove provider.
                    provider_connection_map.erase(provider_connection_map.find(provider->get_connection()));
                    delete provider;
                    providers.erase(provider_iter);
                }
            }

            /*!
             * Set the connection timeout. This is the time before idle connections are
             * disconnected.
             *
             * @param seconds The time in seconds a connection will be maintained while idle.
             */
            void set_idle_connection_timeout( const long seconds ) { connection_timeout = seconds; }

            /*!
             * @return The time in seconds a connection will be maintained while idle.
             */
            long get_idle_connection_timeout( void ) const { return connection_timeout; }

            /*!
             * @return The path the the log files.
             */
            std::string get_log_path( void ) const { return log_path; }

            /*!
             * @return True if logging, false otherwise.
             */
            bool is_logging( void ) const { return logging; }

        protected:
            void cleanup( void );

            virtual void * run_loop( void );

        private:
            std::string get_provider_name( Connection * const connection )
            {
                auto connection_iter = provider_connection_map.find(connection);
                return connection_iter != provider_connection_map.end() ? connection_iter->second : "";
            }

            void destroy_connection( Connection * const connection, const std::string reason = "" )
            {
                // First check if connection belongs to a service provider.
                std::string provider_name = get_provider_name(connection);
                if (provider_name != "")
                    unregister_provider(provider_name, reason);

                // Okay.
                delete connection;
            }

        private:
            static const std::string                    log_path;
            static const bool                           logging;
            Log                                         log;

            static const int                            router_count = 1;

            int                                         next_client_id;
            long                                        connection_timeout;
            std::map<int, ConnectionDescriptor>         connections;
            concurrent::UniqueQueue<ConnectionDescriptor *>
                                                        active_connections;

            std::vector<CommIFDescriptor>               comm_ifs;
            std::vector<concurrent::Thread *>           routers;
            std::map<std::string, ServiceProvider *>    providers;
            std::map<Connection *, std::string>         provider_connection_map;
    };
} }

#endif
