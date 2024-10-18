import 'package:dino_game_leadboard/db/db_helper.dart';
import 'package:dino_game_leadboard/model/player.dart';
import 'package:flutter/material.dart';

class PlayerProvider with ChangeNotifier {
  DBHelper dbHelper = DBHelper();

  Future<void> insertPlayer(Player player) async {
    await dbHelper.insert(player);
    notifyListeners();
  }

  Future<List<Player>> getPlayers() async {
    return await dbHelper.getAllPlayers();
  }

    Future<int> clearPlayers() async {
    int result = await dbHelper.deleteAll();
    notifyListeners(); // Notifica i listener che i dati sono stati aggiornati
    return result;
  }
}
