// ============================================================================
// test_katro.cpp — Tests unitaires rigoureux du Katro tsotra malgache
//
// Référence officielle :
//   Luc TIENNOT, "Les jeux de semailles à Madagascar",
//   Bulletin de l'APMEP / Publimath AVM14034, pages 74 à 76.
//
// Scénarios de tests vérifiés :
//   1. Un semis qui se termine dans une case vide (arrêt du coup).
//   2. Un semis qui se termine dans une case déjà occupée (déclenche un relais).
//   3. Un semis qui se termine sur la rangée intérieure et capture les graines
//      adverses de la même colonne (rangée intérieure adverse non vide).
//   4. Une capture dans la rangée extérieure adverse lorsque toute la rangée
//      intérieure adverse est vide.
//   5. Une tentative de capture alors que la case adverse concernée est vide.
//   6. Un coup qui effectue plusieurs relais et plusieurs captures consécutives.
//   7. Un semis qui traverse le point de départ après avoir parcouru le circuit.
//   8. Un coup dans chaque sens de circulation (horaire et antihoraire).
//   9. La conservation du nombre total de graines (invariance absolue).
//  10. La détection correcte de la fin de partie (victoire par dénuement).
// ============================================================================

#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include "KatroGame.hpp"

using namespace gamehub::core;

static void pass(const char* testName) {
    std::cout << "  [PASS] " << testName << "\n";
}

int main() {
    std::cout << "============================================================\n";
    std::cout << "[TEST] Katro tsotra — Validation algorithmique (Publimath)\n";
    std::cout << "============================================================\n";

    // =========================================================================
    // Test 1 : Un semis qui se termine dans une case vide
    //
    // Configuration : Plateau 4×4.
    // Sud sème depuis 0 (ext, col 0) en sens horaire avec 1 graine.
    // Prochaine case : 4 (int, col 0).
    // Pour éviter toute capture accidentelle à l'arrivée en 4 :
    // Nord a 0 graines en col 0 (case 8 int = 0, case 12 ext = 0).
    // Nord a des graines en case 15 (col 3) pour maintenir son camp.
    // Résultat attendu :
    // Case 0 vidée, case 4 reçoit 1 graine. Case 4 était vide -> coup terminé.
    // Tour passe à Nord (P2).
    // =========================================================================
    {
        KatroGame game(false, 4, 0);
        std::vector<int> b(16, 0);
        b[0]  = 1;  // Sud départ (rangée 0, col 0)
        b[4]  = 0;  // Sud arrivée (rangée 1, col 0, vide)
        b[8]  = 0;  // Nord en face int (vide)
        b[12] = 0;  // Nord en face ext (vide)
        b[15] = 2;  // Nord ailleurs (col 3)
        game.setBoard(b);

        bool ok = game.playTurn(0, 1, KatroDirection::CLOCKWISE);
        assert(ok && "Le coup doit être valide");
        assert(game.getSeedCount(0) == 0 && "Case 0 doit être vidée");
        assert(game.getSeedCount(4) == 1 && "Case 4 (arrivée) doit contenir 1 graine");
        assert(game.getTurn() == 2 && "Le coup s'arrête dans une case vide -> tour passe à Nord (2)");
        assert(!game.isFinished() && "Partie non finie");

        pass("Scénario 1 : Semis terminé dans une case vide (fin de coup)");
    }

    // =========================================================================
    // Test 2 : Un semis qui se termine dans une case déjà occupée (relais)
    //
    // Configuration : Plateau 4×4.
    // Sud sème en sens horaire (CLOCKWISE) sur sa rangée extérieure :
    // Row 0 : col 3 (case 3) -> col 2 (case 2) -> col 1 (case 1) -> col 0 (case 0).
    // On place :
    //   b[3] = 1 (Sud ext, col 3)
    //   b[2] = 1 (Sud ext, col 2, occupée avant dépôt)
    //   b[1] = 0 (Sud ext, col 1, vide)
    //   b[15] = 4 (Nord)
    // 1er semis élémentaire : 1 graine de 3 vers 2.
    // Case 2 contenait 1 graine -> reçoit 1 graine -> compte 2 graines (occupée).
    // Case 2 est sur la rangée extérieure (row 0) -> pas de capture.
    // Relais déclenché : Sud prend les 2 graines de 2 et sème en 1 et 0.
    // Case 1 reçoit 1 graine.
    // Case 0 était vide -> reçoit 1 graine.
    // Case 0 est sur la rangée extérieure -> pas de capture.
    // Case 0 était vide avant dépôt -> le coup s'arrête en case 0 !
    // Résultat attendu : case 3 = 0, case 2 = 0, case 1 = 1, case 0 = 1.
    // =========================================================================
    {
        KatroGame game(false, 4, 0);
        std::vector<int> b(16, 0);
        b[3]  = 1;
        b[2]  = 1;
        b[1]  = 0;
        b[0]  = 0;
        b[15] = 4;
        game.setBoard(b);

        bool ok = game.playTurn(3, 1, KatroDirection::CLOCKWISE);
        assert(ok);
        assert(game.getSeedCount(3) == 0 && "Case 3 vidée");
        assert(game.getSeedCount(2) == 0 && "Case 2 vidée par le relais");
        assert(game.getSeedCount(1) == 1 && "Case 1 a reçu 1 graine");
        assert(game.getSeedCount(0) == 1 && "Case 0 a reçu 1 graine (fin de coup)");
        assert(game.getTurn() == 2 && "Tour passe à Nord");

        pass("Scénario 2 : Semis terminé dans une case occupée (relais réussi)");
    }

    // =========================================================================
    // Test 3 : Arrivée sur la rangée intérieure et capture adverse (Cas B)
    //
    // Configuration : Plateau 4×4.
    // Sud sème depuis 0 (col 0, row 0) en sens horaire avec 1 graine.
    // Case suivante : 4 (col 0, row 1, rangée intérieure Sud).
    // Case 4 a déjà 1 graine.
    // En face de 4 : case 8 (row 2, col 0, rangée intérieure Nord) a 3 graines !
    // Nord a aussi des graines en case 15 (Nord ext).
    // Déroulement :
    // 1) 1 graine de 0 déposée en 4. Case 4 a maintenant 2 graines.
    // 2) Case 4 est sur la rangée intérieure. Nord a des graines en face (case 8 = 3).
    // 3) CAPTURE : case 8 vidée à 0.
    //    Graines en main = case 4 (2 graines) + capturées (3) = 5 graines.
    // 4) Nouveau semis élémentaire depuis 4 en sens horaire : 5, 6, 7, 3, 2.
    //    Case 2 (ext) était vide -> reçoit la 5ème graine et arrête le coup.
    // Résultat attendu :
    //    Case 8 vidée à 0, Sud a gagné 3 graines, Nord a perdu 3 graines.
    // =========================================================================
    {
        KatroGame game(false, 4, 0);
        std::vector<int> b(16, 0);
        b[0]  = 1;
        b[4]  = 1;
        b[8]  = 3;  // Nord en face sur rangée intérieure
        b[15] = 4;  // Nord ailleurs
        game.setBoard(b);

        int totalBefore = game.getTotalSeedsOnBoard();
        int southBefore = game.countPlayerSeeds(KatroPlayer::SOUTH);
        int northBefore = game.countPlayerSeeds(KatroPlayer::NORTH);

        bool ok = game.playTurn(0, 1, KatroDirection::CLOCKWISE);
        assert(ok);

        assert(game.getSeedCount(8) == 0 && "Case 8 de Nord capturée et vidée à 0");
        int southAfter = game.countPlayerSeeds(KatroPlayer::SOUTH);
        int northAfter = game.countPlayerSeeds(KatroPlayer::NORTH);
        assert(northAfter == northBefore - 3 && "Nord a perdu 3 graines");
        assert(southAfter == southBefore + 3 && "Sud a gagné 3 graines");
        assert(game.getTotalSeedsOnBoard() == totalBefore && "Conservation absolue");

        assert(game.getSeedCount(4) == 0 && "Case 4 vidée après capture et relance");
        assert(game.getSeedCount(5) == 1);
        assert(game.getSeedCount(6) == 1);
        assert(game.getSeedCount(7) == 1);
        assert(game.getSeedCount(3) == 1);
        assert(game.getSeedCount(2) == 1 && "Case 2 (arrêt final)");
        assert(game.getTurn() == 2 && "Tour passe à Nord");

        pass("Scénario 3 : Capture sur la rangée intérieure adverse (Cas B)");
    }

    // =========================================================================
    // Test 4 : Capture sur la rangée extérieure adverse quand la rangée
    //          intérieure adverse est TOTALEMENT vide (Cas C)
    //
    // Configuration : Plateau 4×4.
    // Rangée intérieure de Nord (cases 8, 9, 10, 11) : TOUTES VIDES (0).
    // Rangée extérieure de Nord : case 12 (col 0) a 4 graines ! Case 15 a 2 graines.
    // Sud sème depuis 0 (col 0) avec 1 graine -> atterrit en 4 (col 0, int).
    // Toute la rangée intérieure adverse est vide -> Sud peut capturer la case 12 en face !
    // Case 12 vidée à 0.
    // Graines en main = case 4 (1 graine) + capturées (4) = 5 graines.
    // Nouveau semis depuis 4 en sens horaire : 5, 6, 7, 3, 2.
    // =========================================================================
    {
        KatroGame game(false, 4, 0);
        std::vector<int> b(16, 0);
        b[0]  = 1;
        b[4]  = 0;
        // Cases 8, 9, 10, 11 sont à 0 (rangée int Nord vide)
        b[12] = 4;  // Nord ext en face (col 0)
        b[15] = 2;  // Nord ext ailleurs (col 3)
        game.setBoard(b);

        assert(game.isOpponentInnerRowEmpty(KatroPlayer::SOUTH));

        int totalBefore = game.getTotalSeedsOnBoard();
        bool ok = game.playTurn(0, 1, KatroDirection::CLOCKWISE);
        assert(ok);

        assert(game.getSeedCount(12) == 0 && "Case 12 (Nord ext) doit être capturée");
        assert(game.countPlayerSeeds(KatroPlayer::SOUTH) == 1 + 4 && "Sud gagne les 4 graines capturées");
        assert(game.getTotalSeedsOnBoard() == totalBefore && "Conservation absolue");

        pass("Scénario 4 : Capture rangée extérieure adverse quand rangée intérieure vide (Cas C)");
    }

    // =========================================================================
    // Test 5 : Tentative de capture alors que la case adverse concernée est vide
    //
    // Configuration : Plateau 4×4.
    // Sud atterrit en 4 (int, col 0).
    // Nord a des graines en case 9 (int, col 1), mais sa case 8 (int, col 0) est VIDE (0).
    // Nord a aussi sa case 12 (ext, col 0) vide.
    // La rangée intérieure de Nord n'est pas entièrement vide (case 9 a 3 graines).
    // En face de 4 : case 8 = 0 -> pas de capture !
    // Case 4 était vide avant dépôt -> coup terminé en 4 avec 1 graine.
    // =========================================================================
    {
        KatroGame game(false, 4, 0);
        std::vector<int> b(16, 0);
        b[0]  = 1;
        b[4]  = 0;
        b[8]  = 0;  // Nord col 0 int vide
        b[9]  = 3;  // Nord col 1 int non vide
        b[12] = 0;  // Nord col 0 ext vide
        b[15] = 2;  // Nord col 3
        game.setBoard(b);

        assert(!game.isOpponentInnerRowEmpty(KatroPlayer::SOUTH));

        bool ok = game.playTurn(0, 1, KatroDirection::CLOCKWISE);
        assert(ok);

        assert(game.getSeedCount(4) == 1 && "Case 4 a reçu 1 graine");
        assert(game.getSeedCount(8) == 0 && "Case 8 reste à 0");
        assert(game.getSeedCount(9) == 3 && "Case 9 reste intacte");
        assert(game.getTurn() == 2 && "Fin de coup : pas de capture, case était vide");

        pass("Scénario 5 : Case adverse vide en face -> pas de capture, coup terminé");
    }

    // =========================================================================
    // Test 6 : Coup avec plusieurs relais et plusieurs captures consécutives
    //
    // Configuration : Plateau 4×4.
    // Sud joue en sens antihoraire (COUNTER_CLOCKWISE) :
    // Circuit Sud : 0 -> 1 -> 2 -> 3 -> 7 -> 6 -> 5 -> 4 -> 0.
    // Chaîne de relais et captures multiples successives.
    // =========================================================================
    {
        KatroGame game(false, 4, 0);
        std::vector<int> b(16, 0);
        b[0]  = 2;  // Sud départ
        b[2]  = 1;  // Relais en case 2
        b[11] = 2;  // 1ère capture (en face de 7)
        b[8]  = 1;  // 2ème capture (en face de 4)
        b[15] = 5;  // Nord réserve
        game.setBoard(b);

        int totalBefore = game.getTotalSeedsOnBoard();
        int southBefore = game.countPlayerSeeds(KatroPlayer::SOUTH);
        int northBefore = game.countPlayerSeeds(KatroPlayer::NORTH);

        bool ok = game.playTurn(0, 1, KatroDirection::COUNTER_CLOCKWISE);
        assert(ok);

        // Les deux cases adverses doivent être capturées et vidées
        assert(game.getSeedCount(11) == 0 && "Case 11 adverse capturée");
        assert(game.getSeedCount(8) == 0 && "Case 8 adverse capturée");

        int southAfter = game.countPlayerSeeds(KatroPlayer::SOUTH);
        int northAfter = game.countPlayerSeeds(KatroPlayer::NORTH);
        assert(southAfter == southBefore + 3 && "Sud a capturé 2 + 1 = 3 graines");
        assert(northAfter == northBefore - 3 && "Nord a perdu 3 graines");
        assert(game.getTotalSeedsOnBoard() == totalBefore && "Conservation absolue");

        pass("Scénario 6 : Coup enchaînant plusieurs relais et captures multiples");
    }

    // =========================================================================
    // Test 7 : Un semis qui traverse le point de départ après avoir bouclé
    //
    // Configuration : Plateau 4×4 (circuit de 8 cases par joueur).
    // Sud sème depuis 0 avec 9 graines en sens antihoraire.
    // 9 graines sur un circuit de 8 cases : la case 0 de départ DOIT recevoir
    // une graine lors du second tour (ne reste pas vide).
    // =========================================================================
    {
        KatroGame game(false, 4, 0);
        std::vector<int> b(16, 0);
        b[0]  = 9;
        b[14] = 4; // Nord
        game.setBoard(b);

        bool ok = game.playTurn(0, 1, KatroDirection::COUNTER_CLOCKWISE);
        assert(ok);

        assert(game.getSeedCount(0) >= 1 && "La case de départ doit recevoir une graine au bouclage");
        assert(game.getTotalSeedsOnBoard() == 13 && "Conservation totale");

        pass("Scénario 7 : Traversée du point de départ après bouclage du circuit");
    }

    // =========================================================================
    // Test 8 : Un coup dans chaque sens de circulation (horaire et antihoraire)
    // =========================================================================
    {
        // 8a : Sens horaire (CLOCKWISE)
        // Circuit Sud : 0 -> 4 -> 5 -> 6 -> 7 -> 3 -> 2 -> 1 -> 0
        {
            KatroGame game(false, 4, 0);
            std::vector<int> b(16, 0);
            b[0]  = 2; // 2 graines depuis 0 en sens horaire : vont en 4 et 5
            b[15] = 4;
            game.setBoard(b);
            bool ok = game.playTurn(0, 1, KatroDirection::CLOCKWISE);
            assert(ok);
            assert(game.getSeedCount(0) == 0);
            assert(game.getSeedCount(4) == 1 && "Sens horaire : 0 -> 4");
            assert(game.getSeedCount(5) == 1 && "Sens horaire : 4 -> 5");
        }

        // 8b : Sens antihoraire (COUNTER_CLOCKWISE)
        // Circuit Sud : 0 -> 1 -> 2 -> 3 -> 7 -> 6 -> 5 -> 4 -> 0
        {
            KatroGame game(false, 4, 0);
            std::vector<int> b(16, 0);
            b[0]  = 2; // 2 graines depuis 0 en sens antihoraire : vont en 1 et 2
            b[15] = 4;
            game.setBoard(b);
            bool ok = game.playTurn(0, 1, KatroDirection::COUNTER_CLOCKWISE);
            assert(ok);
            assert(game.getSeedCount(0) == 0);
            assert(game.getSeedCount(1) == 1 && "Sens antihoraire : 0 -> 1");
            assert(game.getSeedCount(2) == 1 && "Sens antihoraire : 1 -> 2");
        }

        pass("Scénario 8 : Coups validés dans les deux sens (horaire et antihoraire)");
    }

    // =========================================================================
    // Test 9 : Conservation du nombre total de graines sur une partie entière
    //
    // Configuration : Plateau standard 4×4 avec 2 graines par case (32 graines).
    // On simule 30 coups successifs. La somme des graines DOIT rester constante.
    // =========================================================================
    {
        KatroGame game(false, 4, 2); // 4x4 standard, 2 graines/case = 32 graines
        assert(game.getTotalSeedsOnBoard() == 32);

        for (int step = 0; step < 30 && !game.isFinished(); ++step) {
            int turn = game.getTurn();
            int startCell = -1;
            int startRange = (turn == 1) ? 0 : 8;
            int endRange   = startRange + 8;
            for (int c = startRange; c < endRange; ++c) {
                if (game.getSeedCount(c) > 0) {
                    startCell = c;
                    break;
                }
            }

            if (startCell >= 0) {
                game.playTurn(startCell, turn, KatroDirection::CLOCKWISE);
            }

            assert(game.getTotalSeedsOnBoard() == 32 &&
                   "Invariance absolue : le total des graines doit rester de 32");
        }

        pass("Scénario 9 : Conservation stricte du nombre total de graines (32)");
    }

    // =========================================================================
    // Test 10 : Détection correcte de la fin de partie (victoire)
    //
    // Configuration : Nord n'a plus qu'une case avec 1 graine, qui est capturée.
    // Nord passe à 0 graines.
    // isFinished() doit devenir true, et le gagnant doit être Sud (1).
    // =========================================================================
    {
        KatroGame game(false, 4, 0);
        std::vector<int> b(16, 0);
        b[0] = 1;  // Sud départ
        b[4] = 0;  // Sud int
        b[8] = 1;  // La SEULE graine restante de Nord
        game.setBoard(b);

        assert(game.countPlayerSeeds(KatroPlayer::NORTH) == 1);

        bool ok = game.playTurn(0, 1, KatroDirection::CLOCKWISE);
        assert(ok);

        assert(game.getSeedCount(8) == 0);
        assert(game.countPlayerSeeds(KatroPlayer::NORTH) == 0 && "Nord n'a plus de graines");
        assert(game.isFinished() && "La partie doit être terminée");
        assert(game.getWinner() == 1 && "Le gagnant doit être Sud (1)");

        pass("Scénario 10 : Détection de la fin de partie et désignation du vainqueur");
    }

    // =========================================================================
    // Test 11 : Bot adverse — Sélection d'un coup aléatoire parmi les coups disponibles
    //
    // 1. Sur plateau standard 4×4 : toutes les cases de Nord (8..15) ont des graines.
    //    Le bot doit toujours renvoyer une case valide entre 8 et 15.
    //    Sur 50 tirages, plusieurs cases différentes doivent être sélectionnées
    //    (preuve d'un tirage aléatoire et non statique).
    // 2. Sur un plateau restreint où seules deux cases de Nord ont des graines (ex: 10 et 14),
    //    le bot ne doit tirer qu'exclusivement l'une de ces deux cases.
    // 3. Quand Nord n'a aucune graine disponible, computeBotMove() renvoie -1.
    // =========================================================================
    {
        KatroGame game(true, 4, 2);
        std::set<int> distinctMoves;

        for (int i = 0; i < 50; ++i) {
            int move = game.computeBotMove();
            assert(move >= 8 && move <= 15 && "Le coup du bot doit être dans le camp de Nord (8..15)");
            assert(game.getSeedCount(move) > 0 && "La case choisie doit contenir des graines");
            distinctMoves.insert(move);
        }
        assert(distinctMoves.size() > 1 && "Le bot doit effectuer des choix aléatoires variés parmi les coups disponibles");

        // Cas restreint : seulement les cases 10 et 14 disponibles
        std::vector<int> b(16, 0);
        b[0]  = 2;  // Sud
        b[10] = 3;  // Nord
        b[14] = 1;  // Nord
        game.setBoard(b);

        for (int i = 0; i < 20; ++i) {
            int move = game.computeBotMove();
            assert((move == 10 || move == 14) && "Le bot ne peut choisir qu'une case disponible non vide");
        }

        // Cas vide : aucune graine chez Nord
        std::vector<int> bEmpty(16, 0);
        bEmpty[0] = 5;
        game.setBoard(bEmpty);
        assert(game.computeBotMove() == -1 && "computeBotMove doit renvoyer -1 si aucune case n'est disponible");

        pass("Scénario 11 : Coup aléatoire disponible pour le bot adverse (Nord)");
    }

    // =========================================================================
    // Test 12 : Mode Solo — Cycle de jeu complet avec mise à jour du bot via update()
    //
    // - P1 joue un coup (Sud). Le tour passe à Nord (P2).
    // - L'appel à update() après BOT_DELAY_SEC déclenche automatiquement le coup aléatoire du bot.
    // - Le tour repasse à Sud (1) ou la partie se termine.
    // - La séquence de coups (m_moveSeq) s'incrémente.
    // =========================================================================
    {
        KatroGame soloGame(true, 4, 2);
        assert(soloGame.getTurn() == 1);
        int seqStart = soloGame.getMoveSeq();

        // Sud joue la case 0
        bool ok = soloGame.playTurn(0, 1, KatroDirection::CLOCKWISE);
        assert(ok);
        assert(soloGame.getTurn() == 2 && "Le tour doit passer au bot (Nord, 2)");
        assert(soloGame.getMoveSeq() > seqStart && "La séquence de coup de P1 a été enregistrée");

        int seqAfterP1 = soloGame.getMoveSeq();

        // Simule l'avancement du temps dans la boucle de tick (> BOT_DELAY_SEC)
        soloGame.update(KatroGame::BOT_DELAY_SEC + 0.1f);

        // Le bot doit avoir joué son coup aléatoire
        assert(soloGame.getMoveSeq() > seqAfterP1 && "Le coup du bot doit incrémenter la séquence de déplacement");
        assert(soloGame.getTurn() == 1 || soloGame.isFinished() && "Après le coup du bot, c'est à P1 ou fin de partie");

        pass("Scénario 12 : Exécution automatique du coup du bot en mode solo via update()");
    }

    std::cout << "============================================================\n";
    std::cout << "[SUCCÈS] Tous les 12 scénarios de tests ont réussi avec succès !\n";
    std::cout << "============================================================\n";
    return 0;
}
