#ifndef __SERIALIZER_H__
#define __SERIALIZER_H__

namespace simu
{
    /**
     * Interface pour charger un objet ou de le créer
     */
    class Serializable
    {
        public:
        virtual ~Serializable() {};

        /**
         * @brief Construit l'objet au moment d'être chargé
         * @param Serializer étant le support pour charger l'objet
         */
        virtual void load(Serializer) = 0;

        /**
         * @brief Sauvegarde les données d'objet
         * @param Serializer étant le support pour écrire l'objet
         */
        virtual void save(Serializer) = 0; 
    };

    /**
     * @brief Class permettant de serialiser et deserialiser 
     */
    class Serializer
    {
        public:
            Serializer(const int version) : m_version(version) {};
            virtual ~Serializer() {};
        
        private:
            const int m_version;
    };
}

#endif
