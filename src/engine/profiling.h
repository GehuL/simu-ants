#ifndef __PROFILING_H__
#define __PROFILING_H__

#include <unordered_map>
#include <string>
#include <stack>
#include <chrono>

namespace simu
{
    /**
     * @brief Structure de données pour profiler une partie du code.
     */
    class ProfileData
    {
        public:

        static constexpr int SAMPLE_SIZE = 30;
        
        std::chrono::steady_clock::time_point lastTime;
        std::chrono::duration<double> samples[SAMPLE_SIZE];
        int sampleIdx;

        void reset();

        /**
         * @brief Calcul la moyenne des échantillons.
         */
        std::chrono::duration<double> calculAverage();
        
        /**
         * @brief Calcul le temps écoulé entre le dernier @ref Profile::begin() et au moment de l'appel de cette méthode.
         * @param elapsedTime Temps écoulé depuis le dernier échantillon.
         */
        std::chrono::duration<double> elapsedTime(); 
        double getFrequency();

        bool isOpen;
    };
    
    /**
     * @brief Class pour profiler le code (mesurer le temps d'execution de certaines parties du code).
     * Permet également de faire une moyenne glissante sur les temps d'execution.
     */
    class Profiler
    {
        public:
            static constexpr bool SCOPED = true;
            static constexpr bool UNSCOPED = !SCOPED;

            Profiler();

            /**
             * @brief Démarre le profiling d'une partie du code.
             * @tparam scoped Si true, le profiling est effectué dans un bloc de code et ajoute le profiler courant dans la pile. Appeler @ref end() pour dépiler le profiler. 
             * Sinon, le profiling est effectué sur une seule ligne et n'est pas mis dans la pile. Appeler @ref end(const std::string& name) pour terminer le profiling.
             * @param name Nom de la partie du code à profiler.
             */
            template<bool scoped = SCOPED>
            void begin(const std::string& name);
            
            /**
             * @brief Dépile le profiling actuelle et met à jour le ProfilerData associé.
             */
            void end();

            /**
             * @brief Termine le profiling d'un bloc de code en dehors de la pile.
             */
            void end(const std::string& name);

            /**
             * @brief Récupère les données de profiling d'une partie du code.
             * @return ProfileData* Pointeur vers les données de profiling. Si la partie du code n'a pas été encore profilée, un ProfileData est créé initialisé à zéro.
             */
            ProfileData* getProfile(const std::string& name);
            ProfileData* operator[](const std::string& name);

            /**
             * @brief Réinitialise tous les profilers.
             */
            void resetAll();

        private:
            typedef std::unordered_map<std::string, ProfileData> containerType;
            containerType mProfiles;
            std::stack<containerType::iterator> mStack;
    };
}


#endif 