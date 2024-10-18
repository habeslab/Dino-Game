import 'dart:io';

import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'model/player.dart';
import 'provider/player_provider.dart';
import 'package:sqflite_common_ffi/sqflite_ffi.dart';



void main() {

  if (!kIsWeb && (Platform.isWindows || Platform.isLinux || Platform.isMacOS)) {
    sqfliteFfiInit();
    databaseFactory = databaseFactoryFfi;
  }
  runApp(
    ChangeNotifierProvider(
      create: (context) => PlayerProvider(),
      child: const MyApp(),
    ),

  );
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Dino Game Leaderboard',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      debugShowCheckedModeBanner: false,
      home: const LeaderboardScreen(),
    );
  }
}

class LeaderboardScreen extends StatelessWidget {
  const LeaderboardScreen({super.key});

  @override
  Widget build(BuildContext context) {
    final playerProvider = Provider.of<PlayerProvider>(context);

    return Scaffold(
      appBar: AppBar(
        title: const Row(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Icon(Icons.emoji_events),
            SizedBox(width: 8),
            Text('Dino Game Leaderboard'),
            SizedBox(width: 8),
            Icon(Icons.gamepad),
          ],
        ),
        actions: [
          IconButton(
            icon: const Icon(Icons.add_circle, color: Colors.blue),
            tooltip: 'Add Player',
            onPressed: () {
              Navigator.push(
                context,
                MaterialPageRoute(builder: (context) => AdminScreen()),
              );
            },
          ),
        ],
      ),
      body: FutureBuilder<List<Player>>(
        future: playerProvider.getPlayers(),
        builder: (context, snapshot) {
          if (snapshot.connectionState == ConnectionState.waiting) {
            return const Center(child: CircularProgressIndicator());
          } else if (snapshot.hasError) {
            return const Center(child: Text('Error loading data'));
          } else if (!snapshot.hasData || snapshot.data!.isEmpty) {
            // Mostra un messaggio quando non ci sono dati e fornisci un pulsante per navigare
            return const Center(
              child: Column(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  Text('No data available'),
                ],
              ),
            );
          }

          final players = snapshot.data!;
          return ListView.builder(
            itemCount: players.length,
            itemBuilder: (context, index) {
              final player = players[index];
              return Card(
                elevation: 3,
                margin: const EdgeInsets.symmetric(vertical: 8, horizontal: 16),
                child: ListTile(
                  leading: CircleAvatar(
                    backgroundColor: Colors.blue,
                    child: Text(
                      player.firstName[0] + player.lastName[0],
                      style: const TextStyle(color: Colors.white),
                    ),
                  ),
                  title: Text('${player.firstName} ${player.lastName}'),
                  trailing: Text(
                    '${player.score}',
                    style: TextStyle(
                      fontWeight: FontWeight.bold,
                      color: Colors.green[700],
                    ),
                  ),
                ),
              );
            },
          );
        },
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: () async {
          // Mostra un dialogo di conferma prima di eliminare tutti i giocatori
          bool confirm = await showDialog(
            context: context,
            builder: (context) {
              return AlertDialog(
                title: const Text('Confirm Reset'),
                content: const Text('Are you sure you want to reset the leaderboard? This will delete all players.'),
                actions: [
                  TextButton(
                    onPressed: () => Navigator.of(context).pop(false), // Cancella
                    child: const Text('Cancel'),
                  ),
                  TextButton(
                    onPressed: () => Navigator.of(context).pop(true), // Conferma
                    child: const Text('Reset'),
                  ),
                ],
              );
            },
          );

          // Se l'utente conferma, chiama il metodo per eliminare tutti i giocatori
          if (confirm) {
            await playerProvider.clearPlayers(); // Metodo nel PlayerProvider
            ScaffoldMessenger.of(context).showSnackBar(
              const SnackBar(content: Text('Leaderboard reset successfully!')),
            );
          }
        },
        backgroundColor: Colors.red, // Colore di sfondo del FAB
        child: const Icon(Icons.delete_forever, color: Colors.white), // Icona di reset
      ),
    );
  }
}

class AdminScreen extends StatelessWidget {
  final _formKey = GlobalKey<FormState>();
  final _firstNameController = TextEditingController();
  final _lastNameController = TextEditingController();
  final _scoreController = TextEditingController();

  AdminScreen({super.key});

  @override
  Widget build(BuildContext context) {
    final playerProvider = Provider.of<PlayerProvider>(context, listen: false);

    return Scaffold(
      appBar: AppBar(
        title: const Text('Add Player'),
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Form(
          key: _formKey,
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.stretch,
            children: [
              TextFormField(
                controller: _firstNameController,
                decoration: const InputDecoration(
                  labelText: 'First Name',
                  prefixIcon: Icon(Icons.person),
                  border: OutlineInputBorder(),
                ),
                validator: (value) {
                  if (value == null || value.isEmpty) {
                    return 'Please enter first name';
                  }
                  return null;
                },
              ),
              const SizedBox(height: 16),
              TextFormField(
                controller: _lastNameController,
                decoration: const InputDecoration(
                  labelText: 'Last Name',
                  prefixIcon: Icon(Icons.person_outline),
                  border: OutlineInputBorder(),
                ),
                validator: (value) {
                  if (value == null || value.isEmpty) {
                    return 'Please enter last name';
                  }
                  return null;
                },
              ),
              const SizedBox(height: 16),
              TextFormField(
                controller: _scoreController,
                decoration: const InputDecoration(
                  labelText: 'Score',
                  prefixIcon: Icon(Icons.score),
                  border: OutlineInputBorder(),
                ),
                keyboardType: TextInputType.number,
                validator: (value) {
                  if (value == null || value.isEmpty) {
                    return 'Please enter score';
                  }
                  return null;
                },
              ),
              const SizedBox(height: 20),
              ElevatedButton(
                onPressed: () {
                  if (_formKey.currentState!.validate()) {
                    final player = Player(
                      firstName: _firstNameController.text,
                      lastName: _lastNameController.text,
                      score: int.parse(_scoreController.text),
                    );
                    playerProvider.insertPlayer(player);
                    // Clear the text fields after adding the player
                    _firstNameController.clear();
                    _lastNameController.clear();
                    _scoreController.clear();
                    // Optional: Show a success message
                    ScaffoldMessenger.of(context).showSnackBar(
                      const SnackBar(content: Text('Player added successfully!')),
                    );
                    Navigator.pop(context);
                  }
                },
                child: const Text('Add Player'),
              ),
            ],
          ),
        ),
      ),
    );
  }
}




