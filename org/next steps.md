# do soon
Clean up / organize code
    interface / ux?

# next goals
polish track-laying UX
    track plan
        similar to user as now, but
            doesn't store tracks in tiles
            maintains list, which is iterated through and checked, for collisions
        highlight where 'cursor' is
        starts at factory tile
        Hitting enter cements plan and starts building
            Would be so cool if these get built out slowly
        escape deletes plan and returns to last built track or factory
drive car on track
    manual
        car at constant speed
        has preference for primary and secondary curveTypes
            at fork, will try primary first, then secondary
            hitting up/left/right pushes new primary curveType; prev primary becomes new secondary.
    automatic
        might have to wait for stations or mining or other objectives