# Syngenta fork releases

These releases split the native-memory work in [fork PR #5](https://github.com/syngenta/rgeo/pull/5)
into two independently deployable packages. Both retain the multipolygon hull
support shipped in 3.1.0.1.

| Version | Scope |
| --- | --- |
| 3.1.0.2 | Estimated GEOS geometry/prepared-index memory accounting, balanced ownership and GC-safety fixes. Prepared optimization remains enabled. The old exception callback is unchanged. |
| 3.1.0.3 | All of 3.1.0.2, plus deferred Ruby exceptions after GEOS returns, constructor cleanup and error-detail/status handling. |

The estimates are not exact native sizes. Version 3.1.0.2 intentionally leaves
the C++ exception-memory leak unresolved; it is not a complete native-memory fix.

## Build and validate

From the source tree for the desired version:

```sh
bundle install
bundle exec rake
gem build rgeo.gemspec
```

These are source gems (platform `ruby`). Installation compiles the C extension
and requires GEOS headers/libraries and a compiler, as with 3.1.0.1; consuming the
published package does not require a Git checkout.

Run the complete native memory check for either version:

```sh
bundle exec rake test:valgrind
```

A passing Valgrind suite alone does not establish C++ exception-storage balance.
An independent allocation/free probe on Ruby 4.0.9 / GEOS 3.13.1, using 1,000
parser failures against the installed packages, reports 1,000 allocations and
zero frees for 3.1.0.2, versus 1,000 allocations and 1,000 frees for 3.1.0.3.
That difference is intentional in this staged release; no leak suppressions
were added.

## Publish only after approval

Use the authenticated Gemfury CLI and explicitly select the existing account:

```sh
fury push rgeo-3.1.0.2.gem --account oleksii-leonov
fury push rgeo-3.1.0.3.gem --account oleksii-leonov
```

Do not use an unqualified `gem push`: the gem keeps the upstream name `rgeo`
and is intended for Gemfury, not RubyGems.org. Keep credentials outside source
control. Upload the validated artifacts; if rebuilding, recompute checksums and
update the consumer lockfile before release.

Official references:
[uploading packages](https://gemfury.com/help/upload-packages/) and
[CLI authentication/account selection](https://gemfury.com/guide/cli/authentication/).

## Consumer rollout

1. Publish 3.1.0.2 before merging/deploying the first CUP update.
2. Keep an exact pin in CUP's existing Gemfury source:
   `gem "rgeo", "3.1.0.2"`. Do not use a Git source or a range that can select 3.1.0.3.
3. Verify the published package checksum against the prepared artifact and test
   a clean remote install. A successful local-cache install does not prove upload
   or remote availability.
4. Canary the first version and compare per-process memory, error rates and
   latency on representative geometry workloads.
5. Upgrade the exact pin to 3.1.0.3 in a separate application change after the
   first rollout is assessed. Regenerate the lockfile/license cache and rerun
   related application tests.

Rollback from 3.1.0.3 to 3.1.0.2 retains accounting while removing the new
exception handling. Rollback to 3.1.0.1 removes both fixes. Neither fix alone
establishes the cause of all MTD memory growth.
