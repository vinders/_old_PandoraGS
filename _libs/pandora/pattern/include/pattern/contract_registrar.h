/*******************************************************************************
Author  :     Romain Vinders
License :     GPLv2
*******************************************************************************/
#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <deque>
#include <unordered_map>
#include <mutex>
#include "./contract.h"
#include "./locked.h"

namespace pandora {
  namespace pattern {
    /// @class DirectContractRegistrar
    /// @brief Basic registrations management: register/unregister calls will have to wait until current access to "data()" is closed.
    ///        More efficient than DeferrableContractRegistrar if registrations/unregistrations rarely occur (typically at startup/end of program).
    /// @warning No register/unregister operation is allowed in the data processing (callback/notify/...)!
    ///          -> data processing should never trigger the destruction of a Contract or ContractProvider. See DeferrableContractRegistrar to allow it.
    /// @description This instance should be passed as a shared_ptr to the corresponding ContractProvider(s).
    ///              ex: auto registrar = std::make_shared<DirectContractRegistrar<_RegisteredDataType> >();
    ///                  ContractProvider contractProvider(registrar);
    template <typename _RegisteredDataType, typename _LockType = std::mutex>
    class DirectContractRegistrar final : public ContractListener {
      using Type = DirectContractRegistrar<_RegisteredDataType,_LockType>;
      using registration_map = std::unordered_map<Contract::InternalTicket, _RegisteredDataType>;
      using registered_data_move = typename std::conditional<std::is_class<_RegisteredDataType>::value, _RegisteredDataType&&, _RegisteredDataType>::type;

      DirectContractRegistrar() = default;
      DirectContractRegistrar(const Type&) = delete;
      DirectContractRegistrar(Type&&) = default;
      Type& operator=(const Type&) = delete;
      Type& operator=(Type&&) = default;
      ~DirectContractRegistrar() noexcept { std::lock_guard<_LockType> guard(this->_registrationsLock); }

      // -- getters --

      /// @brief Read all registered data
      inline const LockedRef<registration_map, _LockType> data() const noexcept { 
        return LockedRef<registration_map, _LockType>(this->_registrations, this->_registrationsLock); 
      }

      // -- operations --

      /// @brief Add a new registration (call it right after the contract is created by a ContractProvider)
      inline void create(const Contract& contract, registered_data_move regData) {
        std::lock_guard<_LockType> guard(this->_registrationsLock);
        this->_registrations.emplace(contract.ticket(), std::move(regData));
      }

      // -- event handlers --

      /// @brief Remove a registration on contract destruction (called by ContractProvider)
      inline void onRevoke(const Contract& contract) noexcept override {
        std::lock_guard<_LockType> guard(this->_registrationsLock);
        this->_registrations.erase(contract.ticket());
      }

    private:
      registration_map _registrations;
      mutable _LockType _registrationsLock;
    };

    // ---

    /// @class DeferrableContractRegistrar
    /// @brief Manage registrations and defer them if necessary (to allow non-blocking registrations during a target access)
    /// @description This instance should be passed as a shared_ptr to the corresponding ContractProvider(s).
    ///              ex: auto registrar = std::make_shared<DeferrableContractRegistrar<_RegisteredDataType> >();
    ///                  ContractProvider contractProvider(registrar);
    template <typename _RegisteredDataType, typename _LockType = std::mutex>
    class DeferrableContractRegistrar final : public ContractListener {
      using Type = DeferrableContractRegistrar<_RegisteredDataType,_LockType>;
      using registration_map = std::unordered_map<Contract::InternalTicket, _RegisteredDataType>;
      using registered_data_move = typename std::conditional<std::is_class<_RegisteredDataType>::value, _RegisteredDataType&&, _RegisteredDataType>::type;

      DeferrableContractRegistrar() = default;
      DeferrableContractRegistrar(const Type&) = delete;
      DeferrableContractRegistrar(Type&&) = default;
      Type& operator=(const Type&) = delete;
      Type& operator=(Type&&) = default;
      ~DeferrableContractRegistrar() noexcept { std::lock_guard<_LockType> guard(this->_registrationsLock); }

      // -- getters --

      /// @brief Read all registered data
      inline const LockedRef<registration_map, _LockType> data() const { 
        LockedRef<registration_map, _LockType> lockedData(this->_registrations, this->_registrationsLock)
        _processPendingRequests();
        return std::move(lockedData);
      }

      // -- operations --

      /// @brief Add a new registration (call it right after the contract is created by a ContractProvider)
      inline void create(const Contract& contract, registered_data_move regData) {
        std::lock_guard<_LockType> guard(this->_registrationsLock, std::defer_lock);
        if (guard.try_lock())
          this->_registrations.emplace(contract.ticket(), std::move(regData));
        else {
          std::lock_guard<_LockType> guard(this->_pendingDataLock);
          this->_pendingRegistrations.emplace(contract.ticket(), std::move(regData));
        }
      }

      // -- event handlers --

      /// @brief Remove a registration on contract destruction (called by ContractProvider)
      inline void onRevoke(const Contract& contract) noexcept override {
        std::unique_lock<_LockType> guard(this->_registrationsLock, std::defer_lock);
        if (guard.try_lock()) {
          if (this->_registrations.find() != this->_registrations.end()) {
            this->_registrations.erase(contract.ticket());
          }
          else {
            guard.unlock();
            _cancelPendingRegistration(contract.ticket());
          }
        }
        else {
          std::lock_guard<_LockType> guard(this->_pendingDataLock);
          this->_pendingRevocations.emplace(contract.ticket());
        }
      }

    protected:
      // apply all pending operations (_registrationsLock must be locked!)
      void _processPendingRequests() const {
        std::lock_guard<_LockType> guard(this->_pendingDataLock);

        if (!this->_pendingRevocations.empty())
          _processPendingRevocations();
        if (!this->_pendingRegistrations.empty())
          _processPendingRegistrations();
      }

      // revoke a pending registration
      void _cancelPendingRegistration(const Contract::InternalTicket& ticket) noexcept {
        std::lock_guard<_LockType> guard(this->_pendingDataLock);
        for (auto it = this->_pendingRevocations.begin(); it != this->_pendingRevocations.end(); ++it) {
          if (*it == ticket)
            this->_pendingRevocations.erase(it);
        }
      }

    private:
      // apply pending registrations (_registrationsLock & _pendingDataLock must be locked!)
      void _processPendingRegistrations() const {
        for (auto& reg : this->_pendingRegistrations) {
          if (reg.first.isValid())
            this->_registrations.emplace(reg.first, std::move(reg.second));
        }
        this->_pendingRegistrations.clear();
      }
      // remove revoked registrations (_registrationsLock & _pendingDataLock must be locked!)
      void _processPendingRevocations() const noexcept {
        for (auto& ticket : this->_pendingRevocations)
          this->_registrations.erase(ticket);
        this->_pendingRevocations.clear();
      }

    private:
      registration_map _registrations;
      std::deque<std::pair<Contract::InternalTicket,_RegisteredDataType> > _pendingRegistrations;
      std::deque<Contract::InternalTicket> _pendingRevocations;
      mutable _LockType _registrationsLock;
      mutable _LockType _pendingDataLock;
    };

  }
}
